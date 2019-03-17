#include <cassert>
#include <cerrno>
#include <cstring>
#include "logging/logging/log.h"
#include "net/tcp/tcp_server.h"

enum ServerStatus
{
    SS_CLOSED = 0x1,
    SS_RUNNING = 0x2,
    SS_ERROR  = 0x4,
};

const int32_t MSG_CACHE_SIZE = 10*1024;

TcpServer::TcpServer(const std::string ip,int32_t port,std::string name,int32_t objid,LinkAttr attr)
    :linkid_(SETLINKKEY(objid,0)),generate_(1)
    ,status_(INT32(SS_CLOSED)),port_(port),ip_(ip),name_(name),accept_(NULL),link_attr_(attr)
{
    link_map_.insert(std::make_pair(linkid_, LinkSPtr()));
    //add_func_ = QCOOL::bind(default_add_callback);
    del_func_ = QCOOL::bind(default_del_callback);
}

TcpServer::~TcpServer()
{
    //系统析构前，必须先调用OnStop.
    assert((INT32(SS_CLOSED) == status_));
    assert((link_map_.empty()));
}

void TcpServer::SendData(TcpMessage &msg)
{
    QMutexGuard guard(map_mutex_);
    LinkMap::iterator pos = link_map_.find(msg.key_);
    if (pos != link_map_.end())
    {
        pos->second->SendMessage(msg);
    }
    else
    {
    	LOGERROR("Server:Send Failed!");
    	msg.data_free();//[knull-2]
    }
}

//[knull-2]:not used;
//void TcpServer::SendData(TcpMessageQueue &queue)
//{
//    QMutexGuard guard(map_mutex_);
//    TcpMessageQueue::iterator pos = queue.begin();
//    while (pos != queue.end())
//    {
//        SendData(*pos);
//        ++pos;
//    }
//}

//[knull-2]:not used;
//void TcpServer::SendData(TcpMessageQueue &queue,LinkKey k)
//{
//    QMutexGuard guard(map_mutex_);
//    LinkMap::iterator pos = link_map_.find(k);
//    if (pos != link_map_.end())
//    {
//        pos->second->SendMessage(queue);
//    }
//}

void TcpServer::KickLink(LinkKey k)
{
    QMutexGuard guard(map_mutex_);
    LinkMap::iterator pos = link_map_.find(k);
    if (pos != link_map_.end())
    {
        //pos->second->SendMessage(queue);
        Libuv::Instance().LinkCloseWakeup(k);
    }
}
//--------------------------------------------------
bool TcpServer::OnStart()
{
    //QMutexGuard guard(mutex_);
    if (INT32(SS_CLOSED) != status_)
    {
        return true;
    }
    sockaddr_in addr;
    uv_ip4_addr(ip_.c_str(),port_,&addr);

    if (!accept_)
    {
        uv_tcp_t *paccept = Libuv::Instance().CreateInitTcpObj();
        paccept->data = (void*)&linkid_;
        accept_ = (paccept);
    }

    if (0 != uv_tcp_bind(accept_, (const struct sockaddr*)&addr, 0))
    {
		//_CRT_SECURE_NO_WARNINGS 
         LOGINFO("uv_tcp_bind (%s:%d) failed:%s!", ip_.c_str(), port_,strerror(errno));
        return false;
    }
    if (0 != uv_listen((uv_stream_t*)accept_, SOMAXCONN, LibuvCb::tcp_accept_cb))
    {
         LOGINFO("uv_listen (%s:%d) failed:%s!", ip_.c_str(), port_, strerror(errno));
        return false;
    }
    //Libuv::Instance().LinkAcceptWakeup();
    Libuv::Instance().RegistWakeup();

    status_ = INT32(SS_RUNNING);
    LOGINFO("server(0x%x) started : SS_RUNNING!",GETOBJECTID(linkid_));
    LOGINFO("\tListen on (%s:%d)",ip_.c_str(),port_);
    return true;
}

void TcpServer::OnStop(LinkKey id)
{
    if (id == linkid_)
    {
        status_ = INT32(SS_CLOSED);
        QMutexGuard guard(map_mutex_);
        link_map_.erase(linkid_);
        LinkMap::iterator pos = link_map_.begin();
        while (pos != link_map_.end())
        {
            pos->second->Stop();
            del_func_(id);
            ++pos;
        }
        link_map_.clear();
        uv_close((uv_handle_t*)accept_,LibuvCb::uv_after_tcp_close);
        accept_ = NULL;
    }
    else
    {
        OnStopLink(id);
    }
}

void TcpServer::OnError(LinkKey k)
{
    //mutex;
    OnStopLink(k);
}

void TcpServer::OnStopLink(LinkKey k)
{
    assert((status_ != INT32(SS_CLOSED)));
    map_mutex_.Lock();
    LinkMap::iterator pos = link_map_.find(k);
    if (pos == link_map_.end())
    {
        //assert(0);
        map_mutex_.UnLock();
        return ;
    }
    pos->second->Stop();
    link_map_.erase(pos);
    map_mutex_.UnLock();
    del_func_(k);
}

//OnXXX函数，都是如下特点：epoll线程;调用TcpMananger调用这些函数函数;
LinkKey TcpServer::OnAccept( )
{
    assert((status_ != INT32(SS_CLOSED)));
    uv_tcp_t *stream = Libuv::Instance().CreateInitTcpObj();
    if (0 != uv_accept((uv_stream_s*)accept_, (uv_stream_s*)stream))
    {
        LOGERROR("uv_accept failed!");
        return 0;
    }
    LinkKey linkid = GetNextLinkId();
    LinkSPtr tcplink(new TcpLink(linkid,stream,link_attr_));
    tcplink->SetRecvMsg_Callback(rcv_msg_cb_);

    if (0 != uv_read_start((uv_stream_s*)stream, LibuvCb::buf_alloc, LibuvCb::tcp_read_cb))
    {
        LOGERROR("uv_read_start failed!");
        return 0;
    }
    Libuv::Instance().RegistWakeup();
    {
        QMutexGuard guard(map_mutex_);
        link_map_.insert(std::pair<LinkKey,LinkSPtr>(linkid,tcplink));
    }
    //add_func_(linkid);

    const AddressInfo *info = tcplink->GetAddress(AT_REMOTE);
    //LOGINFO("--accept one link(0x%lx) connected from (%s:%d)!",linkid,info->ip_.c_str(),info->GetPort());
    LOGINFO("--accept one link(%"PRIx64") connected from (%s:%d)!",linkid,info->ip_.c_str(),info->GetPort());
    return linkid;
}

int32_t TcpServer::OnLinkRead(LinkKey k,ssize_t n,const char *d)
{
    assert((status_ != INT32(SS_CLOSED)));
    QMutexGuard guard(map_mutex_);
    LinkMap::iterator pos = link_map_.find(k);
    if (pos != link_map_.end())
    {
        return pos->second->OnRead(n,d);
    }
    else
    {
        LOGERROR("Logic error:Can't find(0x%"PRIx64") in this Server!",k);
        return -2;
    }
}

void TcpServer::OnLinkWrite(LinkKey k)
{
    assert((status_ != INT32(SS_CLOSED)));
    QMutexGuard guard(map_mutex_);
    LinkMap::iterator pos = link_map_.find(k);
    if (pos != link_map_.end())
    {
        pos->second->OnAsyncWrite();
    }
}

LinkKey TcpServer::GetNextLinkId()
{
    //assert((linkid_& MASK32BIT));
    LinkKey key = 0;
    do
    {//0是TcpServer用的,所以肯定不会有问题.
        int32_t id  = generate_.fetch_add(1);
        key = linkid_|id;
    }while (link_map_.find(key) != link_map_.end());
    return key;
}




