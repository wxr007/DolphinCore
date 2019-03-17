#include <cassert>

#include "logging/logging/log.h"
#include "net/tcp/tcp_manager.h"

TcpManager *TcpManager::instance_;
QCOOL::once_flag TcpManager::run_flag_;

//extern void uv_sleep(int32_t = 0);

// void TcpManager::InitHeartBeat(int32_t cycle_sec)
// {
//     {
//         QMutexGuard guard(timer_mutex_);
//         if (heartbeart_timer_)
//         {
//             LOGERROR("InitHeartBeat Again!");
//             return;
//         }
//         heartbeart_timer_ = new HeartBeatTimer(cycle_sec);
//     }    
//     if (heartbeart_timer_)
//     {
//         RegisterTimer(heartbeart_timer_);
//     }
//     else
//     {
//         LOGERROR("No memory to alloc!");
//     }
// }

// void TcpManager::RegistHeartBeat(LinkKey k, HBTimerCallback cb)
// {
//     heartbeart_timer_->Register(k, cb);
// }
// 
// void TcpManager::UnRegistHeartBeat(LinkKey k)
// {
//     heartbeart_timer_->UnRegister(k);
// }

// void TcpManager::RegisterTimer(TimerBase *tb)
// {
//     Timeout t;
//     t.repeat_ = tb->timeout_;
//     t.timeout_ = tb->next_timeout_;
//     tb->timer_id_ = Libuv::Instance().RegisterTimer(t);
//     timers_.insert(std::make_pair(tb->timer_id_,tb));
// }
// 
// void TcpManager::OnTimer(TimerId id)
// {
//     QMutexGuard guard(timer_mutex_);
//     QCOOL::unordered_map<TimerId, TimerBase*>::iterator pos = timers_.find(id);
//     if (pos != timers_.end())
//     {
//         pos->second->OnTimer(id);
//     }
// }

void TcpManager::FetchRecvQueue(std::vector<TcpMessage> &q,int32_t waittime/*millisecond*/)
{
    q.clear();
    QMutexGuard guard(read_msg_mutex_);
    rm_cond_.TimeWait(10);
    q.swap(recv_queue_);
}

void TcpManager::PutRecvQueue(TcpMessageQueue &q)
{
    QMutexGuard guard(read_msg_mutex_);
    recv_queue_.insert(recv_queue_.end(),q.begin(),q.end());
    //if (recv_queue_.size() > 30)//gamesever 不需要这个设置.gateway需要这个设置.
    {
        rm_cond_.Notify();
    }
}

TcpManager::TcpManager():obj_mutex_(),read_msg_mutex_(),rm_cond_(read_msg_mutex_)
{
    recv_queue_.reserve(64*1024);
//    add_func_ = bind(&TcpManager::LinkAdd,this,_1);
    del_func_ = bind(&TcpManager::LinkDel,this,_1);
}

int32_t TcpManager::AddClient(const std::string ip,int32_t port,const std::string name)
{
   SPtrTcpObj cli(new TcpObj);
   cli->obj_id_ = GetNextObjId(TCPOBJ_CLIENT);
   cli->name_ = name;
   cli->client_.reset(new TcpClient(ip, port, name, cli->obj_id_));
   obj_mutex_.Lock();
   prepare_.insert(std::make_pair(cli->obj_id_, cli));
   obj_mutex_.UnLock();
   Libuv::Instance().TcpStartWakeup(SETLINKKEY(cli->obj_id_,0));
   return cli->obj_id_;
}
//如果auth为空，表示不需要鉴权,直接可以连接通过.
int32_t TcpManager::AddServer(const std::string ip,int32_t port,const std::string name,LinkAttr attr)
{
    SPtrTcpObj svr(new TcpObj);
    svr->obj_id_ = GetNextObjId(TCPOBJ_SERVER);
    svr->name_ = name;
    svr->server_.reset(new TcpServer(ip, port, name, svr->obj_id_,attr));
    obj_mutex_.Lock();
    prepare_.insert(std::make_pair(svr->obj_id_,svr));
    obj_mutex_.UnLock();
    Libuv::Instance().TcpStartWakeup(SETLINKKEY(svr->obj_id_,0));
    return svr->obj_id_;
}

//server send data;
int32_t TcpManager::ClientSendData(TcpMessage &msg,int32_t obj)
{
    assert((GetObjType(obj) == TCPOBJ_CLIENT));//[knull-1]
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(obj);
    if (pos != idx_obj_map_.end())
    {
        SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        sp->client_->SendData(msg);
        return TM_ERRNO_OK;
    }
    obj_mutex_.UnLock();
    msg.data_free();//[knull-2]
    LOGINFO("%s:find Object(%x) by linkid(0x%"PRIx64")failed!", __FUNCTION__, GETOBJECTID(msg.key_), msg.key_);
    return TM_ERRNO_NOTFONLINK;
}
int32_t TcpManager::SendData(TcpMessage &msg)
{
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(GETOBJECTID(msg.key_));
    if (pos != idx_obj_map_.end())
    {
        SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        assert(sp);
        switch (GetObjType(sp->obj_id_))
        {
        case TCPOBJ_CLIENT:
        {
            sp->client_->SendData(msg);
            break;
        }
        case TCPOBJ_SERVER:
        {
            sp->server_->SendData(msg);
            break;
        }
        default :
            assert(0);
        }
        msg.clear();
        return TM_ERRNO_OK;
    }
    else
    {
        obj_mutex_.UnLock();
        msg.data_free();//[knull-2]
        LOGINFO("%s:find Object(%x) by linkid(0x%"PRIx64")failed!", __FUNCTION__, GETOBJECTID(msg.key_), msg.key_);
    }
    return TM_ERRNO_NOTFONLINK;
}

void TcpManager::KickLink(LinkKey id)
{
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(GETOBJECTID(id));
    if (pos != idx_obj_map_.end())
    {
    	SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        //Libuv::Instance().LinkCloseWakeup(id);
        assert(sp);
        switch (GetObjType(sp->obj_id_))
        {
        case TCPOBJ_CLIENT:
        {
            sp->client_->KickLink(id);
            break;
        }
        case TCPOBJ_SERVER:
        {
            sp->server_->KickLink(id);
            break;
        }
        default:
            assert(0);
        }
    }
    else
    {
        obj_mutex_.UnLock();
    }
}

void TcpManager::StopObj(int32_t obj)
{
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(obj);
    if (pos != idx_obj_map_.end())
    {
        obj_mutex_.UnLock();
        Libuv::Instance().LinkCloseWakeup(SETLINKKEY(obj,0));
    }
    obj_mutex_.UnLock();
    //delete obj;
}

const AddressInfo* TcpManager::GetAddress(LinkKey k,AddressType t)
{
    const AddressInfo* ptemp = NULL;
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(GETOBJECTID(k));
    if (pos != idx_obj_map_.end())
    {
        SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        assert(sp);
        switch (GetObjType(sp->obj_id_))
        {
        case TCPOBJ_CLIENT:
        {
            ptemp = sp->client_->GetAddress(k,t);
            break;
        }
        case TCPOBJ_SERVER:
        {
            ptemp = sp->server_->GetAddress(k,t);
            break;
        }
        default:
            assert(0);
        }
    }
    else
    {
        obj_mutex_.UnLock();
    }
    return ptemp;
}

//-------------------------------libuv callback-----------------------------------
//libuv回调函数，libuv线程专用.
void TcpManager::OnConnect(LinkKey id)
{
    obj_mutex_.Lock();
    QCOOL::unordered_map<int32_t,SPtrTcpObj>::iterator pos = prepare_.find(GETOBJECTID(id));
    if (pos != prepare_.end())
    {
        SPtrTcpObj sp = pos->second;
        obj_mutex_.UnLock();
        //update;
        prepare_.erase(pos);
        idx_obj_map_.insert(std::make_pair(sp->obj_id_, sp));
        id = sp->client_->OnConnect();
        if (0 != id)
        {
            PutMessage(id, TM_TYPE_CLI_CONNECT_OK);
        }
        else
        {
            PutMessage(id, TM_TYPE_CLI_CONNECT_FAILED);
        }
        //client_objs_.push_back(sp);
        vec_objs_.push_back(sp);
    }
    else
    {
        obj_mutex_.UnLock();
        LOGINFO("%s:find Object(0x%x) by linkid(0x%"PRIx64") in prepare_ failed!",__FUNCTION__, GETOBJECTID(id), id);
    }

}

void TcpManager::OnAccept(LinkKey id)
{
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(GETOBJECTID(id));
    if (pos != idx_obj_map_.end())
    {
        SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        id = sp->server_->OnAccept();
        if (id != 0)
        {
            PutMessage(id, TM_TYPE_SVR_ACCEPT_OK);
        }
        else
        {
            PutMessage(id, TM_TYPE_SVR_ACCEPT_FAILED);
        }
    }
    else
    {
        obj_mutex_.UnLock();
        LOGINFO("%s:find Object(0x%x) by linkid(0x%"PRIx64")failed!",__FUNCTION__, GETOBJECTID(id), id);
    }
}

void TcpManager::OnRead(LinkKey id, ssize_t n, const char *d)
{
    //uv_sleep()
    int32_t ret = 0;
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(GETOBJECTID(id));
    if (pos != idx_obj_map_.end())
    {
        SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        assert(sp);
        switch (GetObjType(sp->obj_id_))
        {
        case TCPOBJ_CLIENT:
        {
            ret = sp->client_->OnLinkRead(id,n,d);
            break;
        }
        case TCPOBJ_SERVER:
        {
            ret = sp->server_->OnLinkRead(id,n,d);
            break;
        }
        default :
            assert(0);
        }
    }
    else
    {
        obj_mutex_.UnLock();
        LOGINFO("%s:find Object(0x%x) by linkid(0x%"PRIx64")failed!",__FUNCTION__, GETOBJECTID(id), id);
    }
    if (ret < 0 )
    {
        OnError(id);
    }
}

void TcpManager::OnWrite(LinkKey id)
{
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(GETOBJECTID(id));
    if (pos != idx_obj_map_.end())
    {
        SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        assert(sp);
        switch (GetObjType(sp->obj_id_))
        {
        case TCPOBJ_CLIENT:
        {
            sp->client_->OnLinkWrite(id);
            break;
        }
        case TCPOBJ_SERVER:
        {
            sp->server_->OnLinkWrite(id);
            break;
        }
        default :
            assert(0);
        }
    }
    else
    {
        obj_mutex_.UnLock();
        LOGINFO("%s:find Object(%x) by linkid(0x%"PRIx64")failed!",__FUNCTION__, GETOBJECTID(id), id);
    }
}

void TcpManager::OnError(LinkKey id)
{
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(GETOBJECTID(id));
    if (pos != idx_obj_map_.end())
    {
		int32_t tmptype = 0;
        SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        assert(sp);
        switch (GetObjType(sp->obj_id_))
        {
        case TCPOBJ_CLIENT:
        {
            sp->client_->OnError(id);
            tmptype = TM_TYPE_CLI_ERROR;
            id = id & (MASK32BIT<<32);
            break;
        }
        case TCPOBJ_SERVER:
        {
            sp->server_->OnError(id);
            tmptype = TM_TYPE_SVR_ERROR;
            break;
        }
        default :
            assert(0);
        }
        if ((id & MASK32BIT) != 0)
        {
            //PutMessage(id, TM_TYPE_LINK_ERROR);
        }
        else
        {
            RemoveObj(id);
            PutMessage(id, tmptype);
        }        
    }
    else
    {
        QCOOL::unordered_map<int32_t,SPtrTcpObj>::iterator ppos = prepare_.find(GETOBJECTID(id));
        if (ppos != prepare_.end())
        {
            SPtrTcpObj sp = ppos->second;
            assert(sp);
            assert (GetObjType(sp->obj_id_) == TCPOBJ_CLIENT);
            prepare_.erase(ppos);
            obj_mutex_.UnLock();
            //
            //del_func_(id);
            PutMessage(id, TM_TYPE_CLI_CONNECT_FAILED);
            LOGINFO("%s:Client object(%x) connect failed!",__FUNCTION__, GETOBJECTID(id));
        }
        else
        {
            obj_mutex_.UnLock();
            LOGINFO("%s:find Object(%x) by linkid(0x%"PRIx64")failed!",__FUNCTION__, GETOBJECTID(id), id);
        }              
    }
}

void TcpManager::RemoveObj(LinkKey id)
{
    QMutexGuard guard(obj_mutex_);
    idx_obj_map_.erase(GETOBJECTID(id));
    std::list<SPtrTcpObj>::iterator pos = vec_objs_.begin();
    while (pos != vec_objs_.end())
    {
        if ((*pos)->obj_id_ == GETOBJECTID(id))
        {
            LOGINFO("remove tcpobj(0x%x)!", (*pos)->obj_id_);
            vec_objs_.erase(pos);            
            return ;
        }
        ++pos;
    }
    assert(0);
}

void TcpManager::OnClose(LinkKey id)
{
    obj_mutex_.Lock();
    OBJITR pos = idx_obj_map_.find(GETOBJECTID(id));
    if (pos != idx_obj_map_.end())
    {
        SPtrTcpObj sp = pos->second.lock();
        obj_mutex_.UnLock();
        assert(sp);
        switch (GetObjType(sp->obj_id_))
        {
        case TCPOBJ_CLIENT:
        {
            sp->client_->OnStop(id);
            id = id & (MASK32BIT<<32);
            break;
        }
        case TCPOBJ_SERVER:
        {
            sp->server_->OnStop(id);
            break;
        }
        default :
            assert(0);
        }
        if ((id & MASK32BIT) != 0)
        {
            //PutMessage(id, TM_TYPE_SYSTEM_STOP);
        }
        else
        {
            RemoveObj(id);
            PutMessage(id, TM_TYPE_SYSTEM_STOP);
        }
    }
    else
    {
        obj_mutex_.UnLock();
        LOGINFO("%s:find Object(%x) by linkid(0x%"PRIx64")failed!",__FUNCTION__, GETOBJECTID(id), id);
    }
}

void TcpManager::OnStart(LinkKey id)
{
    //int32_t msg_type = -1;
    obj_mutex_.Lock();
    QCOOL::unordered_map<int32_t, SPtrTcpObj>::iterator pos = prepare_.find(GETOBJECTID(id));
    if (pos != prepare_.end())
    {
        SPtrTcpObj sp = pos->second;
        assert(sp);
        switch (GetObjType(sp->obj_id_))
        {
        case TCPOBJ_CLIENT:
        {
            obj_mutex_.UnLock();
            sp->client_->SetRecvMsg_Callback(QCOOL::bind(&TcpManager::PutRecvQueue,this,_1));
            sp->client_->InitSelf(add_func_,del_func_);
            if (!sp->client_->OnStart())
            {
                PutMessage(id, TM_TYPE_CLI_START_FAILED);
            }
            else
            {
                LOGINFO("client %x Starting!", sp->obj_id_);
            }            
            break;
        }
        case TCPOBJ_SERVER:
        {
            sp->server_->SetRecvMsg_Callback(QCOOL::bind(&TcpManager::PutRecvQueue, this, _1));
            sp->server_->InitSelf(add_func_,del_func_);
            if (!sp->server_->OnStart())
            {
                PutMessage(id, TM_TYPE_SVR_START_FAILED);
            }
            else
            {
                prepare_.erase(pos);
                idx_obj_map_.insert(std::make_pair(sp->obj_id_, sp));
                vec_objs_.push_back(sp);
                LOGINFO("server %x Started!", sp->obj_id_);
            }
            obj_mutex_.UnLock();
            break;
        }
        default :
            assert(0);
        }//end switch;
    }//end if;
    else
    {
        obj_mutex_.UnLock();
        LOGINFO("%s:find Object(%x) by linkid(0x%"PRIx64")failed!",__FUNCTION__, GETOBJECTID(id), id);
    }//end else;

}
//------------------------辅助函数------------------
int32_t TcpManager::GetNextObjId(int32_t type)
{
    int32_t tmp = 0;
    do
    {
        int32_t id = obj_idx_indx_.fetch_add(1);
        tmp = type|(id & TCPOBJ_ID_MASK);
        if (idx_obj_map_.find(tmp) != idx_obj_map_.end())
        {
            continue;
        }
        if (prepare_.find(tmp) != prepare_.end())
        {
            continue;
        }
        break;
    }while (true);
    return tmp;
}





