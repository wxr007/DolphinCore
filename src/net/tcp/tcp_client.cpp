#include "logging/logging/log.h"
#include "net/tcp/tcp_client.h"

enum ClientStatus
{
    CS_CLOSED = 0x1,
    CS_STARTTING = 0x2,
    CS_RUNNING = 0x4,
    CS_ERROR = 0x8,
    CS_LINKSTOPED = 0x10,
};

TcpClient::TcpClient(const std::string ip, int32_t port, std::string name, int32_t objid)
    :link_id_(SETLINKKEY(objid, 0)), status_(INT32(CS_LINKSTOPED)), port_(port), ip_(ip), name_(name)
{
    //add_func_ = QCOOL::bind(default_add_callback);
    del_func_ = QCOOL::bind(default_del_callback);
}
TcpClient::~TcpClient()
{

}
void TcpClient::SendData(TcpMessage &msg)
{
    if (link_)
    {
        link_->SendMessage(msg);
    }
    else
    {
    	LOGERROR("Client:Send Failed!");
    	msg.data_free();//[knull-2]
    }
}
void TcpClient::KickLink(LinkKey k)
{
    if (link_id_ + 1 == k)
    {//判断是否是TcpClient的链接对象.
        Libuv::Instance().LinkCloseWakeup(k);
    }
}
//OnXXX?????????????????epoll???;????TcpMananger??????Щ????????;
bool TcpClient::OnStart()
{
    if (INT32(CS_LINKSTOPED) != status_ && link_)
    {
        LOGWARN("TcpClient(%p) has start[%x]!", this, status_);
        return true;
    }
    //Connectting;
    uv_tcp_t *p = Libuv::Instance().CreateInitTcpObj();
    sockaddr_in addr;
    uv_ip4_addr(ip_.c_str(), port_, &addr);

    tcp_connect_.data = (void*)&link_id_;
    if (uv_tcp_connect(&tcp_connect_, p, (const struct sockaddr*)&addr, LibuvCb::tcp_connect_cb) != 0)
    {
        LOGERROR("TcpClient(%p) has start!", this);
        return false;
    }
    //Libuv::Instance().LinkConnectWakeup();
    Libuv::Instance().RegistWakeup();
    status_ = INT32(CS_STARTTING);
    LOGINFO("client started : CS_STARTTING!");
    return true;
}
void TcpClient::OnStop(LinkKey id)
{
    if (!link_)
    {
    	LOGINFO("Has stoped(0x%"PRIx64")!",link_id_ + 1);
    	return ;
    }
    if (id == link_id_)
    {
        status_ = INT32(CS_CLOSED);
        link_->Stop();
        del_func_(id+1);
    }
    else
    {
        status_ = INT32(CS_LINKSTOPED);
        OnStopLink(id);
        del_func_(id);
    }
    link_.reset();
    LOGINFO("%s:Stop link(0x%"PRIx64")!",__FUNCTION__,link_id_ + 1);
}
void TcpClient::OnStopLink(LinkKey)
{
    link_->Stop();
    LOGINFO("%s:Stop link(0x%"PRIx64")!",__FUNCTION__,link_id_ + 1);
}
LinkKey TcpClient::OnConnect()
{
    if (status_ != INT32(CS_STARTTING))
    {
        //logging;
        LOGERROR("Connect (%s:%d) error !", ip_.c_str(), port_);
        status_ = INT32(CS_CLOSED);
        return 0;
    }

    LinkKey linkid = link_id_ + 1;
    link_.reset(new TcpLink(linkid, (uv_tcp_t*)(tcp_connect_.handle)));
    link_->SetRecvMsg_Callback(rcv_msg_cb_);

    uv_read_start(tcp_connect_.handle, LibuvCb::buf_alloc, LibuvCb::tcp_read_cb);

    Libuv::Instance().RegistWakeup();
    status_ = INT32(CS_RUNNING);
    //add_func_(linkid);
    LOGINFO("client (0x%"PRIx64") connect (%s:%d) OK : CS_RUNNING!",linkid, ip_.c_str(), port_);
    return linkid;
}
int32_t TcpClient::OnLinkRead(LinkKey, ssize_t n, const char *d)
{
    if (link_)
    {
        return link_->OnRead(n, d);
    }
    else
    {
        return -1;
    }
}
void TcpClient::OnLinkWrite(LinkKey)
{
    if (link_)
    {
        link_->OnAsyncWrite();
    }
}
void TcpClient::OnError(LinkKey k)
{
    status_ = INT32(CS_LINKSTOPED);
    link_->Stop();
    link_.reset();
    del_func_(k);
    LOGINFO("%s:Stop link(0x%"PRIx64")!",__FUNCTION__,link_id_ + 1);
}
