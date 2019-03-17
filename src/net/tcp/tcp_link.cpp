#include <cassert>
#include "logging/logging/log.h"
#include "net/tcp/tcp_link.h"

enum LinkStatus
{
    LS_CLOSED = 0,
    LS_CLOSING = 1,
    LS_OPENED = 2,
    LS_OPENNING = 3,
};

enum WriteStatus
{
    WS_PREPARE = 0,
    WS_WRITING = 1,
    WS_WRITED = 2,
};

// extern StreamCounter g_counter_;

void default_add_callback()
{
    LOGWARN("Add LinkId Callback Not Set!");
}
void default_del_callback()
{
    LOGWARN("Delete LinkId Callback Not Set!");
}

const int32_t PARSE_PACKET_LEAST_SIZE_V15 = 6;//Server1.5;
const int32_t PARSE_PACKET_LEAST_SIZE_V20 = 4;//Server2.0;
const int32_t PACKET_MAX_SIZE = 10*1024*1024;
const int32_t PACKET_MIN_SIZE = 8;

inline bool PacketSizeCheck(int32_t sz)
{
    if (sz < PACKET_MIN_SIZE || sz > PACKET_MAX_SIZE)
    {
        LOGERROR("This packet size(%d) error!",sz);
        return false;
    }
    else
    {
        return true;
    }
}

#ifdef _SERVER20_
int32_t TcpLink::parse_need_size_ = PARSE_PACKET_LEAST_SIZE_V20;
#else
int32_t TcpLink::parse_need_size_ = PARSE_PACKET_LEAST_SIZE_V15;
#endif

TcpLink::TcpLink(LinkKey id,uv_tcp_t *link,LinkAttr attr)
    :tcp_link_(link), linkid_(id), status_(INT32(LS_OPENED)),link_attr_(attr)
{
    link->data = (void*)&linkid_;
    LOGINFO("TcpLink(0x%lx) opened!",linkid_);
    LibuvCb::GetRemoteAddress(link,remote_info_);
    LibuvCb::GetLocalAddress(link,local_info_);
//如果定义了该宏,那么需要打开Nagle;系统默认是打开的
#ifdef TCP_DELAY_OPEN
#else//Makefile默认是没有定义该宏的,所以应该走下面:即关闭Nagle.
    if (!LibuvCb::set_tcp_nodelay(link))
    {
    	LOGERROR("Set (0x%"PRIx64")nodelay failed!",id);
    }
#endif//TCP_DELAY_OPEN
}

TcpLink::~TcpLink()
{
    assert((status_ == INT32(LS_CLOSED)));
    for(size_t i=0; i!= write_queue_.size();++i)
    {
    	write_queue_.at(i).data_free();
    }
}

void TcpLink::Stop()
{
    status_ = INT32(LS_CLOSED);
    uv_close((uv_handle_t*)tcp_link_, LibuvCb::uv_after_tcp_close);
    tcp_link_ = NULL;
    LOGINFO("TcpLink(0x%lx) closed!",linkid_);
}

//void TcpLink::SendMessage(TcpMessageQueue &q)
//{
//    {
//        QMutexGuard guard(write_mutex_);
//        write_queue_.insert(write_queue_.end(),q.begin(),q.end());
//    }
//    Libuv::Instance().LinkWriteWakeup(linkid_);//[CPU]
//    q.clear();//[knull-2]:save成功,clear就可以了.
//}

void TcpLink::SendMessage(TcpMessage &tmp)
{
    {
        QMutexGuard guard(write_mutex_);
        write_queue_.push_back(tmp);
    }
    Libuv::Instance().LinkWriteWakeup(linkid_);//[CPU]
    tmp.clear();//[knull-2]:save成功,clear就可以了.
}

int32_t TcpLink::GetOnePacketSize(const char *d)
{
#define GETPACKETFLAGVALUE() (*(d+5))//4+1;
    int32_t hdsz = parse_need_size_;
    int32_t data_size = *((int32_t *)d);
    if (GETPACKETFLAGVALUE() & (1 << 7))
    {
        hdsz += 4;
    }
    return ROUNDNUM8(data_size+hdsz);
}

int32_t TcpLink::Preprocess(ssize_t nread,const char *data)
{
    LOGDEBUG("GateWay Pre-Process");
    //1-
    int32_t need = GW_AUTHORITY_SIZE - gw_attr.write_;
    char *pos = gw_attr.buf_ + gw_attr.write_;
    if (nread < need )
    {
        memcpy(pos,data,nread);
        gw_attr.write_ += nread;
        return nread;
    }
    //enough;
    memcpy(pos,data,need);
    gw_attr.write_ = GW_AUTHORITY_SIZE;
    gw_attr.is_auth_ = true;
    //check;
    if (strstr(gw_attr.buf_,GW_AUTHORITY_PACKATE) == NULL)
    {
        LOGERROR("auth failed:should src[%s] != [%s]",GW_AUTHORITY_PACKATE,gw_attr.buf_);
        return -1;
    }
    else
    {
        LOGINFO("Auth OK!");
    }
    return need;
}

int32_t TcpLink::OnRead(ssize_t nread,const char *data)
{
    if (link_attr_ == LAT_GATEWAY && !(gw_attr.is_auth_))
    {

        int32_t diff = Preprocess(nread,data);
        if (diff < 0)
        {
            return -1;
        }
        assert(nread >= diff);
        nread -= diff;
        data += diff;
        if (!nread)
        {
            return 0;
        }
    }
    TcpMessageQueue read_queue_;
    //int32_t read_data_size  = nread;//for g_counter_;
    //QMutexGuard guard(read_mutex_);
    int32_t left_size = INT32(pre_msg_.size());
    //int32_t data_size = 0;
    int32_t need_size = 0;
    TcpMessage msg;
	//LOGDEBUG("OnRead left_size %d nread %zd!", left_size, nread);
    if (left_size > 0)
    {
		int32_t append_size = 0;
		
        if (left_size + nread < parse_need_size_)
        {
            pre_msg_.append(data,nread);
            LOGDEBUG("Not enough data:%d+%zd < least_size(4)!",left_size,nread);
            return 0;
        }

        if (left_size < parse_need_size_)
        {
            append_size = parse_need_size_ - left_size;
            pre_msg_.append(data, append_size);
            data += append_size;
            nread -= append_size;
            left_size = INT32(pre_msg_.size());
			LOGDEBUG("OnRead left_size %d nread %zd need_size (%d)!", left_size, nread, need_size);
        }

        //need_size = PACKET_SIZE(data_size);
        need_size = GetOnePacketSize(pre_msg_.c_str());
        if (!PacketSizeCheck(need_size))
        {
            return -2;
        }
        append_size = need_size-left_size;
        if (append_size > nread)
        {//内容不够;
            pre_msg_.append(data,nread);
            LOGDEBUG("OnRead Not enough data:%d+%zd< need(%d)!",left_size,nread,need_size);
            return 0;
        }
        //先将left中的处理掉;
        pre_msg_.append(data,append_size);
        data += append_size;
        nread -= append_size;
        //Message *msg = NewMessage(data_size);
        //msg.data_ = malloc(need_size);
        msg.data_new(need_size);
        msg.dlen_ = need_size;
        msg.key_ = linkid_;
        memcpy(msg.data_, pre_msg_.c_str(), need_size);
        read_queue_.push_back(msg);
        msg.clear();
        pre_msg_.clear();
        //msgcount++;
    }
    while (true)
    {
        if (nread < parse_need_size_)
        {
            pre_msg_.append(data,nread);
            break;
        }
        //memcpy(&data_size,data,4);
        need_size = GetOnePacketSize(data);
        if (!PacketSizeCheck(need_size))
        {
            return -3;
        }
        if (nread < need_size)
        {
            pre_msg_.append(data,nread);
            break;
        }
        //Message *msg = NewMessage(data_size);
        //memcpy(msg,data,need_size);
        //msg.data_ = malloc(need_size);
        msg.data_new(need_size);
        msg.dlen_ = need_size;
        msg.key_ = linkid_;
        memcpy(msg.data_, data, need_size);
        read_queue_.push_back(msg);
        msg.clear();

        data += need_size;
        nread -= need_size;
        assert((nread >= 0));
    }
    rcv_msg_cb_(read_queue_);
    return 0;
//     g_counter_.CounterRead(INT32(read_queue_.size()),read_data_size);
}

void TcpLink::OnAsyncWrite()
{
    if (write_queue_.empty())
    {
        return ;
    }

    TcpMessageQueue queue;
    {
        QMutexGuard guard(write_mutex_);
        queue.swap(write_queue_);
    }
    
    int32_t msize = INT32(sizeof(WriteInfo) + (queue.size() - 1)*sizeof(uv_buf_t));
    WriteInfo *info = (WriteInfo *)qmalloc(msize);
    info->buf_num_ = INT32(queue.size());
    //int32_t bytes = 0;
    for (size_t i = 0; i != queue.size(); ++i)
    {
        info->buf_[i].base = (char*)queue.at(i).data_;
        info->buf_[i].len = queue.at(i).dlen_;
        queue.at(i).clear();
        //bytes += info->buf_[i].len;
    }

    info->key_ = linkid_;
    uv_write_t *req = &info->req_;
    if (uv_write(req, (uv_stream_s*)tcp_link_, info->buf_, UINT32(queue.size()), LibuvCb::after_write_cb))
    {//如果write失败,是直接返回的,需要释放这些内存.
    	req->handle = (uv_stream_s*)tcp_link_;
    	LibuvCb::after_write_cb(req,0);
        LOGERROR("write message failed!");
    }
//     g_counter_.CounterWrite(INT32(queue.size()),bytes);
}

//void TcpLink::OnAsyncClose()
//{
//    uv_close((uv_handle_t*)tcp_link_, NULL);
//    tcp_link_ = NULL;
//}











