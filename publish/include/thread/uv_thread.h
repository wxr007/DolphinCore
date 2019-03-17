#ifndef _UV_THREAD_H__
#define _UV_THREAD_H__

//仅仅用于qthread.cpp
#include <cstring>
#include <cassert>

#include <uv.h>

#include "thread/qthr_sync.h"
struct ThreadInfo
{
    int64_t thrid_idx_;//线程id的索引，可以将数值变小一点.s
    uv_thread_t thrid_;//真正的线程id.
    //char thrid_str_[64];
};

typedef ThreadInfo* PThreadInfo;

const int32_t QUEUEU_MASK = 0xff;
const int32_t QUEUUE_SIZE = 128;
struct ThreadInfoQueue
{
    ThreadInfo queue_[QUEUUE_SIZE];//mask 0xff;
    QMutex  mutex_;
    int32_t idx_;
    uv_once_t once_;
};

static ThreadInfoQueue thr_info_queue_;

static void InitThreadInfoQueue_once()
{
    for (int32_t i=0; i!=QUEUUE_SIZE;++i)
    {
        ThreadInfo &thr = thr_info_queue_.queue_[i];
        memset(&thr,0,sizeof(ThreadInfo));
        thr.thrid_idx_ = -1;
    }
    thr_info_queue_.idx_ = 0;
}

inline void InitThreadInfoQueue()
{
    return uv_once(&thr_info_queue_.once_,InitThreadInfoQueue_once);
}

static PThreadInfo InitCurrentThreadInfo()
{
    InitThreadInfoQueue();
    uv_thread_t thrid = uv_thread_self();
    {//mutex domain;
        QMutexGuard guard(thr_info_queue_.mutex_);
        for (int32_t i=0; i!=QUEUUE_SIZE;++i)
        {
            ThreadInfo &thr = thr_info_queue_.queue_[i];
            if (thr.thrid_idx_ !=-1 && (0 != uv_thread_equal(&(thr.thrid_),&thrid)))
            {//find this threadInfo obj;
                return &thr;
            }
        }
        //not find ;
        int32_t old_idx = thr_info_queue_.idx_;
        do
        {
            ThreadInfo &thr = thr_info_queue_.queue_[old_idx];
            if (thr.thrid_idx_ ==-1)
            {//not use;
                thr.thrid_idx_ = old_idx;
                thr.thrid_ = thrid;
                thr_info_queue_.idx_ = old_idx;
                return &thr;
            }
            old_idx = (old_idx+1)&QUEUEU_MASK;
        }while(old_idx != thr_info_queue_.idx_);
    }
    //创建太多线程了（超过128个）.可能存在logic-bug，导致没有清理.
    return NULL;
}



static void ClearThreadInfo(PThreadInfo pthr)
{
    //1-check;
    int64_t id = pthr->thrid_idx_;
    assert((id >= 0 && id < QUEUUE_SIZE));
    //2-do;
    QMutexGuard guard(thr_info_queue_.mutex_);
    ThreadInfo &thr = thr_info_queue_.queue_[id];
    assert((pthr==&thr));
    assert((0 != uv_thread_equal(&(thr.thrid_),&(pthr->thrid_))));
    memset(&thr,0,sizeof(ThreadInfo));
    thr.thrid_idx_ = -1;
}










#endif //_UV_THREAD_H__






