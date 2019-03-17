
#ifndef _TCP_MANAGER_H__
#define _TCP_MANAGER_H__


//#include <memory> //c++11;
#include <string>
#include <vector>
#include <list>
//#include <unordered_map>
//#include <mutex>//c++11;once_flag,call_once;
//#include <functional>
#include "utils/smart_ptr.h"
#include "utils/unordered.h"
#include "utils/once_call.h"
#include "utils/function.h"
#include "utils/atomic.h"

#include "net/tcp/uv_ex.h"
#include "net/tcp/tcp_client.h"
#include "net/tcp/tcp_server.h"
#include "thread/qthr_sync.h"
#include "net/tcp/uv_timer.h"

struct TcpObj
{
    int32_t obj_id_;//为0.
    std::string name_;//不是必须提供,默认为空.
    QCOOL::shared_ptr<TcpClient> client_;
    QCOOL::shared_ptr<TcpServer> server_;
};

const int32_t TCPOBJ_CLIENT = (1<<24);
const int32_t TCPOBJ_SERVER = (1<<25);
const int32_t TCPOBJ_ID_MASK = 0xffffff;

typedef QCOOL::shared_ptr<TcpObj> SPtrTcpObj;
typedef QCOOL::weak_ptr<TcpObj>   WPtrTcpObj;

const int32_t TM_ERRNO_OK = 0;
const int32_t TM_ERRNO_NOTFONLINK = 0x1;



class TcpManager
{
    friend class LibuvCb;
    friend class Libuv;
public ://构造.
    //释放资源，要小心.
    ~TcpManager()
    {
        delete instance_;
    }

    void InitHeartBeat(int32_t cycle_sec = 60);
    void RegistHeartBeat(LinkKey,HBTimerCallback cb);
    void UnRegistHeartBeat(LinkKey k);

    void InitSelf(const LinkUpdateFunc &add,const LinkUpdateFunc &del)
    {
        add_func_ = add;
        del_func_ = del;
//        add_func_ = bind(&TcpManager::LinkAdd,this,_1);
//        del_func_ = bind(&TcpManager::LinkDel,this,_1);
    }

    //不用指针，用引用；指针，那么外部可以delete.
    static TcpManager& Instance()
    {
        QCOOL::call_once(run_flag_,&TcpManager::Construct);
        return *instance_;
    }
    //如果waittime为0,表示一直等待，直到收到消息;
    //否则，时间一到就退出;(和select/epoll类似)
    void FetchRecvQueue1(std::vector<TcpMessage> &q,int32_t waittime = 0/*millisecond*/)
    {
        q.clear();
        QMutexGuard guard(read_msg_mutex_);
        if (waittime == 0)
        {        
            while (recv_queue_.empty())
            {
                rm_cond_.Wait();
            }
        }
        else
        {
            if (recv_queue_.empty())
            {
                rm_cond_.TimeWait(waittime);
            }            
        }
        q.swap(recv_queue_);
    }

    void FetchRecvQueue(std::vector<TcpMessage> &q,int32_t waittime = 0/*millisecond*/);
//    {
//        q.clear();
//        QMutexGuard guard(read_msg_mutex_);
//        rm_cond_.TimeWait(50);
//        q.swap(recv_queue_);
//    }

    void StopObj(int32_t obj);
public ://其它接口.
    //返回objid(int32_t);
    int32_t AddClient(const std::string ip,int32_t port,const std::string name);
    //返回objid(int32_t);
    int32_t AddServer(const std::string ip,int32_t port,const std::string name,LinkAttr attr = INT32(LAT_NONE));
    void KickLink(LinkKey id);
    int32_t SendData(TcpMessage &msg);
    int32_t ClientSendData(TcpMessage &msg,int32_t obj);
    const AddressInfo* GetRemoteAddr(LinkKey k)
    {
        return GetAddress(k,AT_REMOTE);
    }
    const AddressInfo* GetLocalAddr(LinkKey k)
    {
        return GetAddress(k,AT_LOCAL);
    }
    const AddressInfo* GetAddress(LinkKey k,AddressType);
    void WriteWakeup(LinkKey k)
    {
        Libuv::Instance().LinkWriteWakeup(k);
    }
    void WriteWakeup(std::vector<LinkKey>& kq)
    {
        Libuv::Instance().LinkWriteWakeup(kq);
    }
    //[timer]暂时,仅仅内部使用,不给外部用.
    //void RegisterTimer(TimerBase *);
    //void OnTimer(TimerId id);
    //knull:FUCK TODO;
    void Stop();
	//knull:FUCK TODO；
    void DeleteClient(const std::string name){}
    void DeleteServer(const std::string name){}
private :
//    void LinkAdd(LinkKey id)
//    {
//        TcpMessage msg(TM_TYPE_CLI_CONNECT_OK);
//        msg.key_ = id;
//        QMutexGuard guard(read_msg_mutex_);
//        recv_queue_.push_back(msg);
//    }

    void LinkDel(LinkKey id)
    {
        TcpMessage msg(TM_TYPE_SYSTEM_STOP);
        msg.key_ = id;
        QMutexGuard guard(read_msg_mutex_);
        recv_queue_.push_back(msg);
    }
    void PutMessage(LinkKey id,int32_t type)
    {
        TcpMessage msg;
        msg.key_ = id;
        msg.type_ = type;
        QMutexGuard guard(read_msg_mutex_);
        recv_queue_.push_back(msg);
    }
private ://for libuv callback;

    void OnConnect(LinkKey id);
    void OnAccept(LinkKey id);
    void OnRead(LinkKey id, ssize_t n, const char *d);
    void OnWrite(LinkKey id);
    void OnError(LinkKey id);
    void OnClose(LinkKey id);
    void OnStart(LinkKey id);
private ://辅助函数.
    int32_t GetNextObjId(int32_t type);
    int32_t GetObjType(int32_t objid)
    {
        return (objid&(~TCPOBJ_ID_MASK))&MASK32BIT;
    }
    void PutRecvQueue(TcpMessageQueue &q);
//    {
//        QMutexGuard guard(read_msg_mutex_);
//        recv_queue_.insert(recv_queue_.end(),q.begin(),q.end());
//        if (recv_queue_.size() > 50)
//        {
//            rm_cond_.Notify();
//        }
//    }
    void RemoveObj(LinkKey id);
private ://构造函数.
    TcpManager();
    static void Construct()
    {
        instance_ = new TcpManager;
    }
private :
    //std::string proc_name_;

    //knull:FUCK TODO-先用mutex，后期可以改为rwlock.
    QMutex obj_mutex_;
    typedef QCOOL::unordered_map<int32_t,WPtrTcpObj>::iterator OBJITR;
    QCOOL::unordered_map<int32_t,WPtrTcpObj> idx_obj_map_;
    QCOOL::unordered_map<int32_t,SPtrTcpObj> prepare_;
//    std::list<SPtrTcpObj> server_objs_;
//    std::list<SPtrTcpObj> client_objs_;
    std::list<SPtrTcpObj> vec_objs_;

    LinkUpdateFunc add_func_;
    LinkUpdateFunc del_func_;

    //knull:FUCK TODO:可能不需要atomic
    QCOOL::atomic<uint16_t> obj_idx_indx_;

    QMutex read_msg_mutex_;
    QCondition rm_cond_;
    std::vector<TcpMessage> recv_queue_;

    //[timer]
    //QMutex timer_mutex_;
    //QCOOL::unordered_map<TimerId, TimerBase*> timers_;
    //HeartBeatTimer *heartbeart_timer_;

    static TcpManager *instance_;
    static QCOOL::once_flag run_flag_;
};



#endif //_TCP_MANAGER_H__



