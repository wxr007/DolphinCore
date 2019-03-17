#include <assert.h>
#include "logging/logging/log.h"
#include "net/tcp/uv_ex.h"
#include "net/tcp/tcp_manager.h"

uv_once_t Libuv::once_;
Libuv * Libuv::instance_;
//FILE *fplog = fopen("1.log","w");
FILE *fplog = stdout;

const int32_t TIMER_INIT = 0;
const int32_t TIMER_STOP = 1;
const int32_t TIMER_CLOSE = 2;

using std::vector;

Libuv::Libuv()
{
    uv_loop_init(&loop_);
    uv_async_init(&loop_, &evt_regist_, NULL);
    //uv_async_init(&loop_, &timer_register_, LibuvCb::async_timer_register);//[timer]
    //timer_status_ = TIMER_INIT;
    uv_async_init(&loop_, &obj_async_, LibuvCb::async_tcp_start);
    uv_async_init(&loop_, &link_write_, LibuvCb::async_tcp_write_cb);
    uv_async_init(&loop_, &link_close_, LibuvCb::async_tcp_close_cb);
    uv_async_init(&loop_, &on_stop_, LibuvCb::libuv_close);
    thread_ = NULL;
}

bool Libuv::Start()
{
    thread_ = new QThread(QCOOL::bind(&Libuv::uv_run_temp,this),"libuv_thr");
    if (thread_)
    {
        if (thread_->Start())
        {
            LOGINFO("libuv thread start OK!");
            return true;
        }
    }
    LOGERROR("libuv thread start Failed!");
    return false;
}
//[timer]
// void Libuv::CloseTimer()
// {
//     TimerMap::iterator pos = timer_queue_.begin();
//     TimerMap::iterator end = timer_queue_.end();
//     UVTimer *timer = NULL;
//     while (pos != end)
//     {
//         timer = pos->second;
//         uv_close((uv_handle_t*)&timer->timer_,0);
//         qfree(timer);
//         ++pos;
//     }
//     timer_queue_.clear();
//     timer_status_ = TIMER_CLOSE;
//     uv_async_send(&on_stop_);
// }
// 
// void Libuv::StopTimer()
// {
//     TimerMap::iterator pos = timer_queue_.begin();
//     TimerMap::iterator end = timer_queue_.end();
//     UVTimer *timer = NULL;
//     while (pos != end)
//     {
//         timer = pos->second;
//         uv_timer_stop(&timer->timer_);
//         ++pos;
//     }
//     timer_queue_.clear();
//     timer_status_ = TIMER_STOP;
//     uv_async_send(&on_stop_);
// }

void Libuv::OnStop()
{
//     if (!timer_queue_.empty())
//     {
//         if (TIMER_INIT == timer_status_)
//         {
//             //StopTimer();
//             return;
//         }
//         else
//         {
//             //if (TIMER_STOP == timer_status_)
//             assert((TIMER_STOP == timer_status_));
//             //CloseTimer();
//             return;
//         }
//     }
//    uv_close((uv_handle_t*)&timer_register_, NULL);
    uv_close((uv_handle_t*)&obj_async_, NULL);
    uv_close((uv_handle_t*)&evt_regist_, NULL);
    uv_close((uv_handle_t*)&link_write_, NULL);
    uv_close((uv_handle_t*)&link_close_, NULL);
    uv_close((uv_handle_t*)&on_stop_, NULL);
}

void Libuv::Stop()
{
    //Wakeup();
    {
        uv_async_send(&on_stop_);
        //2-等待线程退出：
        thread_->Join();
        //3-close loop;
        if (0 != uv_loop_close(&loop_))
        {
            LOGWARN("uv_loop_close failed!");
        }
        else
        {
            LOGINFO("uv_loop_close OK!");
        }
    }
    delete thread_;
    thread_ = NULL;
    LOGINFO("Libuv exit!");
    //epoll_thread_.Join();
}

void Libuv::uv_run_temp()
{
    LOGINFO("libuv running!");
    uv_run(&loop_,UV_RUN_DEFAULT);
    LOGINFO("libuv stoped!");
}

uv_tcp_t* Libuv::CreateInitTcpObj()
{
    uv_tcp_t *p = (uv_tcp_t *)qmalloc(sizeof(uv_tcp_t));
    uv_tcp_init(&loop_,p);
    LOGDEBUG("Create uv_tcp_t(%p)!",p);
    return p;
}

void Libuv::DestoryTcpObj(uv_tcp_t* h)
{
    qfree(h);
    LOGDEBUG("Destory uv_tcp_t(%p)!",h);
}

//call by other thread;
void Libuv::TcpStartWakeup(LinkKey k)
{
    //locker;
    QMutexGuard guard(obj_mutex_);
    obj_async_queue_.push_back(k);
    uv_async_send(&obj_async_);
}
//[timer]
// int64_t Libuv::RegisterTimer(Timeout &t)
// {
//     QMutexGuard guard(timer_mutex_);
//     t.idx_ = ++timer_idx_;
//     pre_reg_timer_.push_back(t);
//     uv_async_send(&timer_register_);
//     return timer_idx_;
// }
// //[timer]
// void Libuv::TimersRegistInLoop()
// {
//     vector<Timeout> tmp;
//     {
//         QMutexGuard guard(timer_mutex_);
//         pre_reg_timer_.swap(tmp);
//         assert(pre_reg_timer_.size() == 0);
//     }
//     vector<Timeout>::const_iterator cpos = tmp.begin();
//     vector<Timeout>::const_iterator cend = tmp.end();
//     while (cpos != cend)
//     {
//         RegistOneTImer(*cpos);
//         ++cpos;
//     }
// }
// //[timer]
// void Libuv::RegistOneTImer(const Timeout &t)
// {
//     UVTimer * timer = reinterpret_cast<UVTimer *>(qmalloc(sizeof(UVTimer)));
//     if (!timer)
//     {
//         LOGERROR("No memory to alloc!");
//         return;
//     }
//     timer->timer_.data = (void*)timer;
//     timer->idx_ = t.idx_;
// 
//     int32_t ret = 0;    
//     if (0 == (ret = uv_timer_init(&loop_, &(timer->timer_))))
//     {
//         if (0 == (ret = uv_timer_start(&(timer->timer_), LibuvCb::async_timer_cb, t.timeout_, t.repeat_)))
//         {
//             
//             timer_queue_.insert(std::make_pair(t.idx_,timer));
//             LOGDEBUG("Register timer(%"PRIx64")OK!first timeout=%d'ms!Repeat timeout = %d'ms!",t.idx_,t.timeout_,t.repeat_);
//             uv_async_send(&evt_regist_);
//         }
//         else
//         {
//             LOGERROR("uv_timer_start failed(%"PRIx64"):errno=%d;timeout(%d:%d)ms!", t.idx_, ret, t.timeout_, t.repeat_);
//             qfree(timer);
//         }
//     }
//     else
//     {
//         LOGERROR("uv_timer_init failed(%"PRIx64"):errno=%d;timeout(%d:%d)!", t.idx_, ret, t.timeout_, t.repeat_);
//         qfree(timer);
//     }
// }

// void Libuv::LinkConnectWakeup()
// {
//     //locker;
//     uv_async_send(&evt_regist_);
// }
// 
// void Libuv::LinkAcceptWakeup()
// {
//     //locker;
//     uv_async_send(&evt_regist_);
// }

void Libuv::RegistWakeup()
{
    uv_async_send(&evt_regist_);
}

void Libuv::LinkWriteWakeup(LinkKey k)
{
    //locker;
    QMutexGuard guard(write_mutex_);
    write_queue_.push_back(k);
    uv_async_send(&link_write_);
}

void Libuv::LinkWriteWakeup(std::vector<LinkKey>& kq)
{
    //locker;
    QMutexGuard guard(write_mutex_);
    write_queue_.insert(write_queue_.end(),kq.begin(),kq.end());
    uv_async_send(&link_write_);
}

void Libuv::LinkCloseWakeup(LinkKey k)
{
    //locker;
    QMutexGuard guard(close_mutex_);
    close_queue_.push_back(k);
    uv_async_send(&link_close_);
}

//call by epoll thread only;
void Libuv::TcpObjectEvent()
{
    if (obj_async_queue_.empty())
    {
        return ;
    }
    obj_temp_.clear();
    {//lock for obj_async_queue_;
        QMutexGuard guard(obj_mutex_);
        obj_temp_.swap(obj_async_queue_);
    }
    Itr cpos = obj_temp_.begin();
    while (cpos != obj_temp_.end())
    {
        TcpManager::Instance().OnStart(*cpos);
        ++cpos;
    }
}

void Libuv::LinkWriteEvent()
{
    write_temp_.clear();
    {//lock for obj_async_queue_;
        QMutexGuard guard(write_mutex_);
        write_queue_.swap(write_temp_);
    }
    Itr cpos = write_temp_.begin();
    while (cpos != write_temp_.end())
    {
        TcpManager::Instance().OnWrite(*cpos);
        ++cpos;
    }
}

void Libuv::LinkCloseEvent()
{
    close_temp_.clear();
    {//lock for obj_async_queue_;
        QMutexGuard guard(close_mutex_);
        close_temp_.swap(close_queue_);
    }
    Itr cpos = close_temp_.begin();
    while (cpos != close_temp_.end())
    {
        TcpManager::Instance().OnClose(*cpos);
        ++cpos;
    }
}
//-----------------------------------LibuvCb-----------------------------------
#define LINKKEYGET(d) (*((LinkKey*)d))
void LibuvCb::after_write_cb(uv_write_t* req, int status)
{
    WriteInfo* info = (WriteInfo*)req;
    if (status != 0)
    {
        LOGERROR("after_write_cb failed (%d:%s)!",status,uv_strerror(status));//[knull-4]
        TcpManager::Instance().OnError(info->key_);
    }
    for (size_t i = 0; i != info->buf_num_; ++i)
    {
        buf_free(&(info->buf_[i]));
    }
    qfree(info);
}

void LibuvCb::tcp_accept_cb(uv_stream_t* server, int status)
{
    if (status != 0)
    {
        LOGERROR("tcp_accept_cb failed(%d:%s)!",status,uv_strerror(status));//[knull-4]
        TcpManager::Instance().OnError(LINKKEYGET(server->data));
        return;
    }
    TcpManager::Instance().OnAccept(LINKKEYGET(server->data));
}

void LibuvCb::tcp_connect_cb(uv_connect_t* req, int status)
{
    //uv_stream_t* stream = (uv_stream_t*)req->handle;
    if (status != 0)
    {
        LOGERROR("tcp_connect_cb failed(%d:%s)!",status,uv_strerror(status));//[knull-4]
		uv_close((uv_handle_t*)req->handle, LibuvCb::uv_after_tcp_close);
		req->handle = NULL;
        TcpManager::Instance().OnError(LINKKEYGET(req->data));
        return;
    }
    TcpManager::Instance().OnConnect(LINKKEYGET(req->data));
}

void LibuvCb::tcp_read_cb(uv_stream_t* stream,ssize_t nread,const uv_buf_t* buf)
{
    if (nread < 0)
    {
        //assert(nread == UV_EOF);
        LOGERROR("read error:%ld:%s!",nread,uv_strerror((int)nread));//[knull-4]
        qfree(buf->base);
        TcpManager::Instance().OnError(LINKKEYGET(stream->data));
        return;
    }
    TcpManager::Instance().OnRead(LINKKEYGET(stream->data), nread, buf->base);
    qfree(buf->base);
}

void LibuvCb::async_tcp_write_cb(uv_async_t* handle)
{
    Libuv::Instance().LinkWriteEvent();
}

void LibuvCb::async_tcp_close_cb(uv_async_t* handle)
{
    Libuv::Instance().LinkCloseEvent();
}

void LibuvCb::async_tcp_start(uv_async_t* handle)
{
    Libuv::Instance().TcpObjectEvent();
}

void LibuvCb::uv_after_tcp_close(uv_handle_t* handle)
{
    Libuv::Instance().DestoryTcpObj((uv_tcp_t*)handle);
    handle = NULL;
}

void LibuvCb::libuv_close(uv_async_t* handle)
{
    Libuv::Instance().OnStop();
}

//[timer]
// void LibuvCb::async_timer_register(uv_async_t *)
// {
//     Libuv::Instance().TimersRegistInLoop();
// }
// 
// //[timer]
// void LibuvCb::async_timer_cb(uv_timer_t *handle)
// {
//     UVTimer *ptimer = (UVTimer *)(handle->data);
//     TcpManager::Instance().OnTimer(ptimer->idx_);
//     if (!uv_is_active((uv_handle_t*)handle))
//     {//remove timer;
//         uv_close((uv_handle_t*)handle,0);
//         Libuv::Instance().RegistWakeup();
//     }
// }

bool LibuvCb::set_tcp_nodelay(uv_tcp_t *handle)
{
	return (0 == uv_tcp_nodelay(handle, 1));
}

#ifdef WIN32
#include <winsock.h> 
#pragma comment(lib, "wsock32")  
#else
#include <sys/socket.h>
#endif
void LibuvCb::GetAddress(uv_tcp_t* stream, std::string &rip, int32_t &rport)
{
#ifdef WIN32 
    int32_t fd = INT32(stream->socket);
#else
    int32_t fd = stream->io_watcher.fd;
#endif
    sockaddr_in addr;
    socklen_t sz = sizeof(addr);
    if (getpeername(fd, (sockaddr*)&addr, &sz) < 0)
    {
        rport = -1;
    }
    else
    {
        char buf[32] = { 0 };
        uv_ip4_name(&addr, buf, 32);
        rip = buf;
        rport = ntohs(addr.sin_port);
    }
}

void LibuvCb::GetRemoteAddress(uv_tcp_t* stream,AddressInfo &info)
{
#ifdef WIN32
    int32_t fd = INT32(stream->socket);
#else
    int32_t fd = stream->io_watcher.fd;
#endif
    socklen_t sz = sizeof(info.addr_);
    if (getpeername(fd, (sockaddr*)&info.addr_, &sz) < 0)
    {
        info.addr_.sin_port = -1;
    }
    else
    {
        char buf[32] = { 0 };
        uv_ip4_name(&info.addr_, buf, 32);
        info.ip_ = buf;
        info.addr_.sin_port = ntohs(info.addr_.sin_port);
    }
}

void LibuvCb::GetLocalAddress(uv_tcp_t* stream,AddressInfo &info)
{
#ifdef WIN32
    int32_t fd = INT32(stream->socket);
#else
    int32_t fd = stream->io_watcher.fd;
#endif
    socklen_t sz = sizeof(info.addr_);
    if(getsockname(fd,(sockaddr*)&info.addr_,&sz) != 0 )
    {
        info.addr_.sin_port = -1;
    }
    else
    {
        char buf[32] = { 0 };
        uv_ip4_name(&info.addr_, buf, 32);
        info.ip_ = buf;
        info.addr_.sin_port = ntohs(info.addr_.sin_port);
    }
}




