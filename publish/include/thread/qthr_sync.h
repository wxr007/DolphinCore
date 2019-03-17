
#ifndef _QTHR_SYNC_H__
#define _QTHR_SYNC_H__

//#define _USE_STD_LOCK_
#ifdef _USE_STD_LOCK_
#include <mutex>
struct QMutex
{
    QMutex()
    {
    }
    ~QMutex()
    {
    }

    void Lock()
    {
        mutex_.lock();
    }
    void UnLock()
    {
        mutex_.unlock();
    }
    std::mutex mutex_;
};

struct QMutexGuard
{
    QMutexGuard(QMutex &m):lock_(m.mutex_)
    {
    }
    ~QMutexGuard()
    {
    }
    std::unique_lock<std::mutex> lock_;
};

#define QMutexGuard(x) error "Missing guard object name"

#include <condition_variable>

struct QCondition
{
    QCondition()
    {
    }
    ~QCondition()
    {
    }
    void Notify()
    {
        cond_.notify_one();
    }
    void NotifyAll()
    {
        cond_.notify_all();
    }
    void Wait(QMutexGuard &g)
    {
        cond_.wait(g.lock_);
    }
    //超时时间单位为毫秒.
    void TimeWait(int64_t timeout/*ms*/,QMutexGuard &g)
    {
        cond_.wait_for(g.lock_,std::chrono::milliseconds(timeout));
    }
    //std::mutex &mutex_;
    std::condition_variable cond_;
};

//---------------------------------------------libuv 的锁---------------------------------------
#else
#include <uv.h>
struct QMutex
{
    QMutex()
    {
        uv_mutex_init(&(mutex_));
    }
    ~QMutex()
    {
        uv_mutex_destroy(&(mutex_));
    }

    void Lock()
    {
        uv_mutex_lock(&mutex_);
    }
    void UnLock()
    {
        uv_mutex_unlock(&mutex_);
    }
    uv_mutex_t mutex_;
};

struct QMutexGuard
{
    QMutexGuard(QMutex &m):mutex_(m.mutex_)
    {
        uv_mutex_lock(&mutex_);
    }
    ~QMutexGuard()
    {
        uv_mutex_unlock(&mutex_);
    }
    uv_mutex_t &mutex_;
};

#define QMutexGuard(x) error "Missing guard object name"

struct QCondition
{
    QCondition(QMutex &m):mutex_(m.mutex_)
    {
        uv_cond_init(&cond_);
    }
    ~QCondition()
    {
        uv_cond_destroy(&cond_);
    }
    void Notify()
    {
        uv_cond_signal(&cond_);
    }
    void NotifyAll()
    {
        uv_cond_broadcast(&cond_);
    }
    void Wait()
    {
        uv_cond_wait(&cond_,&mutex_);
    }
    //超时时间单位为毫秒.
    void TimeWait(int64_t timeout/*ms*/)
    {
        timeout = timeout*1000*1000;
        uv_cond_timedwait(&cond_,&mutex_,timeout);
    }
    uv_cond_t cond_;
    uv_mutex_t &mutex_;
};
#endif

#endif //_QTHR_SYNC_H__
