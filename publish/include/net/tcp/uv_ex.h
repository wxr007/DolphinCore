
#ifndef _UV_EX_H__
#define _UV_EX_H__

#include <uv.h>
//#include <unordered_set>
#include <vector>
#include <list>
#include "utils/unordered.h"
#include "define/common.h"
#include "thread/qthread.h"
#include "mem/qmemory.h"

// typedef int64_t TimerId;
// struct Timeout
// {
//     int32_t timeout_;//第一次timeout时间;(ms)
//     int32_t repeat_; //重复timeout时间;'=0'表示不需要重复.(ms)
//     TimerId idx_;    //这个值,由Libuv填充.
// };
// 
// struct UVTimer
// {
//     TimerId idx_;
//     uv_timer_t timer_;
// };

class Libuv
{
public :
    ~Libuv();

    void Stop();

    bool Start();

    //void OnStop();

    static Libuv& Instance()
    {
        uv_once(&once_,NewSelf);
        return *instance_;
    }

    //call by other thread;
    void TcpStartWakeup(LinkKey k);
    void LinkConnectWakeup();
    void LinkAcceptWakeup();
    void LinkWriteWakeup(LinkKey k);
    void LinkWriteWakeup(std::vector<LinkKey>& kq);
    void LinkCloseWakeup(LinkKey );
    void RegistWakeup();
    //call by epoll thread only;
    void TcpObjectEvent();
    void LinkWriteEvent();
    void LinkCloseEvent();
    void OnStop();
    //这里直接是malloc,free;后期，也许可以用个pool缓冲.
    uv_tcp_t* CreateInitTcpObj();
    void DestoryTcpObj(uv_tcp_t*);
    //uv_async_t* CreateInitAsyncObj();
    //[timer]
    //TimerId RegisterTimer(Timeout &);//会返回一个注册timer的唯一id,libuv生成.
    //void TimersRegistInLoop();
    
private :
    Libuv();
    void uv_run_temp();
    static void NewSelf()
    {
        instance_ = new Libuv;
    }
    //[timer]
    //void RegistOneTImer(const Timeout &t);
    //[timer]
    //void CloseTimer();
    //void StopTimer();
private :
    typedef std::vector<LinkKey> LinkKeyQueue;
    typedef LinkKeyQueue::const_iterator CItr;
    typedef LinkKeyQueue::iterator Itr;
    typedef LinkKeyQueue::reverse_iterator RItr;
    QMutex obj_mutex_;
    uv_async_t obj_async_;//其他线程要建立连接(AddClient/AddServer),唤醒libuv线程来注册事件.
    LinkKeyQueue obj_async_queue_;
    LinkKeyQueue obj_temp_;

    uv_async_t evt_regist_;//将fd注册到epoll上去,需要唤醒;

    uv_async_t link_write_;//其他线程写事件,仅仅放到队列;然后唤醒libuv线程,真正的去写fd.
    QMutex write_mutex_;
    LinkKeyQueue write_queue_;
    LinkKeyQueue write_temp_;

    QMutex close_mutex_;
    uv_async_t link_close_;//其他线程想关闭fd,会唤醒libuv线程,来关闭fd.
    LinkKeyQueue close_queue_;
    LinkKeyQueue close_temp_;

    uv_async_t on_stop_;//其他线程stop libuv,会唤醒libuv线程来清理自己.
    //[timer]
    //uv_async_t timer_register_;//wake up and register timer;
    //QMutex timer_mutex_;
    //std::vector<Timeout>  pre_reg_timer_;
    //typedef QCOOL::unordered_map<TimerId, UVTimer*> TimerMap;
    //TimerMap timer_queue_;
    //TimerId timer_idx_;//每次注册timer,都自增1.理论上应该够了
    int32_t timer_status_;

    uv_loop_t  loop_;
    QThread  *thread_;

    static Libuv *instance_;
    static uv_once_t once_;
};


class LibuvCb
{
public :
    //下面的所有静态函数，都是epoll线程调用;用于设置libuv的回调函数.
    static void buf_free(uv_buf_t* buf)
    {
        //LOGINFO("-->buf_free %p",buf->base);
        qfree(buf->base);
    }

    static void buf_alloc(uv_handle_t* handle,size_t suggested_size,uv_buf_t* buf)
    {
        buf->base = static_cast<char*>( qmalloc(suggested_size));
#ifdef WIN32
        buf->len = (ULONG)suggested_size;
#else
        buf->len = suggested_size;
#endif
        
        //LOGINFO("<--buf_alloc %p",buf->base);
    }

    static void uv_after_tcp_close(uv_handle_t* handle);

    static void uv_after_async_close(uv_handle_t* handle);

    static void tcp_accept_cb(uv_stream_t* server, int status);

    static void tcp_connect_cb(uv_connect_t* req, int status);

    static void tcp_read_cb(uv_stream_t* stream,ssize_t nread,const uv_buf_t* buf);

    static void after_write_cb(uv_write_t* req, int status);

    static void async_tcp_write_cb(uv_async_t* handle);

    static void async_tcp_close_cb(uv_async_t* handle);

    static void async_tcp_start(uv_async_t* handle);

    static void libuv_close(uv_async_t* handle);

    static void GetAddress(uv_tcp_t* stream, std::string &rip, int32_t &rport);
    static void GetRemoteAddress(uv_tcp_t* stream,struct AddressInfo &info);
    static void GetLocalAddress(uv_tcp_t* stream,struct AddressInfo &info);
    //[timer]
    //static void async_timer_cb(uv_timer_t *);
    //static void async_timer_register(uv_async_t *);
	//即关闭Nagle；
	static bool set_tcp_nodelay(uv_tcp_t *handle);
};


#endif //_UV_EX_H__