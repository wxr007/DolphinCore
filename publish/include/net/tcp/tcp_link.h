#ifndef _TCP_LINK_H__
#define _TCP_LINK_H__

#include <vector>
//#include <memory>
#include "utils/smart_ptr.h"
#include "utils/function.h"
#include "net/tcp/uv_ex.h"

typedef std::vector<TcpMessage> TcpMessageQueue;
typedef QCOOL::function<void(TcpMessageQueue &)> RecvMsgCallback;
typedef QCOOL::function<void(LinkKey)> LinkUpdateFunc;
void default_add_callback();
void default_del_callback();

struct WriteInfo
{
    uv_write_t req_;
    LinkKey    key_;
    int32_t    buf_num_;
    uv_buf_t   buf_[1];
};

enum AddressType
{
    AT_REMOTE = 0,
    AT_LOCAL  = 1,
};

struct AddressInfo
{
    sockaddr_in addr_;
    std::string ip_;
    int32_t GetPort() const
    {//port为主机字节序.
        return addr_.sin_port;
    }
    uint32_t GetIntAddr() const
    {
        return UINT32(addr_.sin_addr.s_addr);
    }
};

class TcpLink : public noncopyable
{
public :
    //由Server/Client创建的时候，提供这些参数;其中，都是初始化好了的.
    TcpLink(LinkKey id,uv_tcp_t *link,LinkAttr attr = INT32(LAT_NONE));

    ~TcpLink();

    void Stop();

    void SendMessage(TcpMessageQueue &);

    void SendMessage(TcpMessage &);

    void SetRecvMsg_Callback(const RecvMsgCallback &cb)
    {
        rcv_msg_cb_ = cb;
    }
    const AddressInfo* GetAddress(AddressType t) const
    {
        if (t == AT_REMOTE)
        {
            return &remote_info_;
        }
        else
        {
            return &local_info_;
        }
    }
public ://private函数，仅仅提供给epoll线程调用的.
    //epoll线程read事件,执行该函数;
    int32_t OnRead(ssize_t nread,const char *);
    int32_t Preprocess(ssize_t nread,const char *data);

    //epoll线程,async write事件执行该函数;
    void OnAsyncWrite();

    //epoll线程,async close事件执行该函数;
    void OnAsyncClose();
private :
    int32_t GetOnePacketSize(const char *d);
private :
    uv_tcp_t *tcp_link_;
    LinkKey   linkid_;   
    
    int32_t status_;
    AddressInfo remote_info_;
    AddressInfo local_info_;
    std::string pre_msg_;

    RecvMsgCallback rcv_msg_cb_;

    //write queue ;
    QMutex write_mutex_;
    TcpMessageQueue write_queue_;
    //
    LinkAttr link_attr_;
    GWConnecter gw_attr;
    //
    static int32_t parse_need_size_;
};

typedef QCOOL::shared_ptr<TcpLink> LinkSPtr;

#endif //_TCP_LINK_H__
