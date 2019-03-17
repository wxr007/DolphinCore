#ifndef _TCP_CLIENT_H__
#define _TCP_CLIENT_H__

#include "net/tcp/tcp_link.h"

class TcpClient
{
public :
    TcpClient(const std::string ip,int32_t port,std::string,int32_t objid);
    ~TcpClient();
    void SendData(TcpMessage &);
	void KickLink(LinkKey k);
    const AddressInfo*  GetAddress(LinkKey k,AddressType t)
    {
        return link_->GetAddress(t);
    }
    void InitSelf(const LinkUpdateFunc &add,const  LinkUpdateFunc &del)
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
    //OnXXX函数，都是如下特点：epoll线程;调用TcpMananger调用这些函数函数;
    bool OnStart();
    void OnStop(LinkKey id);
    void OnStopLink(LinkKey);
    LinkKey OnConnect();
    int32_t OnLinkRead(LinkKey, ssize_t, const char *);
    void OnLinkWrite(LinkKey);
    void OnError(LinkKey);
public :
    void SetRecvMsg_Callback(const RecvMsgCallback &cb)
    {
        //rcv_msg_cb_ = std::move(cb);
        rcv_msg_cb_ = cb;
    }
private :
    LinkSPtr link_;
    uv_connect_t tcp_connect_;
    LinkKey link_id_;//tcpclient的linkid.
    int32_t status_;
    int32_t port_;
    std::string ip_;
    std::string name_;
    RecvMsgCallback rcv_msg_cb_;
    LinkUpdateFunc add_func_;
    LinkUpdateFunc del_func_;
};


#endif //_TCP_CLIENT_H__

