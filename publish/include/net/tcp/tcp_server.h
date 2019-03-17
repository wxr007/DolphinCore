
#ifndef _TCP_SERVER_H__
#define _TCP_SERVER_H__

#include <memory>
//#include <atomic>
//#include <unordered_map>
#include "utils/unordered.h"
#include "utils/atomic.h"
#include "net/tcp/tcp_link.h"


class TcpServer : public noncopyable
{
public :
    TcpServer(const std::string ip,int32_t port,std::string,int32_t objid,LinkAttr attr = INT32(LAT_NONE));

    ~TcpServer();

    void SendData(TcpMessage &);

//    void SendData(TcpMessageQueue &);//[knull-2]:not used;

//    void SendData(TcpMessageQueue &queue,LinkKey k);//[knull-2]:not used;
	
	void KickLink(LinkKey k);

    const AddressInfo* GetAddress(LinkKey k,AddressType t)
    {
        LinkMap::iterator pos = link_map_.find(k);
        if (pos != link_map_.end())
        {
            return  pos->second->GetAddress(t);
        }
        else
        {
            return NULL;
        }
    }

    void InitSelf(const LinkUpdateFunc &add,const LinkUpdateFunc &del)
    {
        if (add)
        {
            add_func_ = add;
        }
        if (del)
        {
            del_func_ = del;
        }
    }

    //void RecvMessage(TcpMessageQueue &);

    //OnXXX函数，都是如下特点：epoll线程;调用TcpMananger调用这些函数函数;
    bool OnStart();

    void OnStop(LinkKey id);

    void OnStopLink(LinkKey);

    LinkKey OnAccept();

    int32_t OnLinkRead(LinkKey ,ssize_t ,const char *);

    void OnLinkWrite(LinkKey);

    void OnError(LinkKey);

public :

//    //仅仅是触发断链事件;
//    void CloseLink(LinkKey );
//    //仅仅删除该链接.
//    void RemoveLink(LinkKey );
    //callback function for link;
    void SetRecvMsg_Callback(const RecvMsgCallback &cb)
    {
        //rcv_msg_cb_ = std::move(cb);
        rcv_msg_cb_ = cb;
    }

    LinkKey GetNextLinkId();
private :
    typedef QCOOL::unordered_map<LinkKey,LinkSPtr> LinkMap;

    LinkKey linkid_;//objid+(0);
    int32_t status_;

    int32_t port_;
    std::string ip_;
    std::string name_;
    QMutex map_mutex_;
    LinkMap link_map_;
    //std::shared_ptr<uv_tcp_t> accept_;
    uv_tcp_t *accept_;

    RecvMsgCallback rcv_msg_cb_;

    LinkUpdateFunc add_func_;
    LinkUpdateFunc del_func_;

    LinkAttr link_attr_;

    QCOOL::atomic<uint32_t> generate_;
};

#endif //_TCP_SERVER_H__
