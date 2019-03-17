
#include <assert.h>
#include "logging/logging/log.h"
#include "thread/qthread.h"

#include "define/common.h"
#include "thread/uv_thread.h"

QThread::QThread(ThrFunc func,std::string thrname)
    : is_joined_(true)
{
    thr_data_.thr_func_ = func;
    thr_data_.thr_name_ = thrname;
    thr_data_.thread_ptr_ = NULL;
    //LOGDEBUG("Thr(idx=%ld) start!",thread_ptr_->thrid_idx_);
}

QThread::~QThread()
{
    QMutexGuard guard(mutex_);
    if (!is_joined_)
    {
        is_joined_ = true;
        assert (thr_data_.thread_ptr_);
        LOGDEBUG("Thr(idx=%ld) exit!",thr_data_.thread_ptr_->thrid_idx_);
        uv_thread_join(&(thr_data_.thread_ptr_->thrid_));
        ClearThreadInfo(thr_data_.thread_ptr_);
        thr_data_.thread_ptr_ = NULL;
    }
}

int32_t QThread::Join()
{
    QMutexGuard guard(mutex_);
    if (is_joined_)
    {
        return 0;
    }
    is_joined_ = true;

    assert (thr_data_.thread_ptr_);
    LOGDEBUG("Thr(idx=%ld) Join!",thr_data_.thread_ptr_->thrid_idx_);
    int32_t r = uv_thread_join(&(thr_data_.thread_ptr_->thrid_));
    ClearThreadInfo(thr_data_.thread_ptr_);
    thr_data_.thread_ptr_ = NULL;
    return r;
}

bool QThread::Start()
{
    uv_thread_t tid;
    is_joined_ = false;
    return (0 == uv_thread_create(&tid,thread_start,(void*)&thr_data_));
}

int32_t QThread::CurrentThreadIdx() const
{
    assert((thr_data_.thread_ptr_!=NULL));
    return INT32(thr_data_.thread_ptr_->thrid_idx_);
}

void QThread::thread_start(void *arg)
{
    ThreadData *thrdata = (ThreadData *)arg;
    thrdata->thread_ptr_ = InitCurrentThreadInfo();
    int64_t idx = thrdata->thread_ptr_->thrid_idx_;
    std::string name = thrdata->thr_name_;
    LOGDEBUG("Thr(idx=%ld)(%s) start!",idx,name.c_str());
    SetThreadName(thrdata->thr_name_.c_str());
    thrdata->thr_func_();
    LOGDEBUG("Thr(idx=%ld)(%s) over!",idx,name.c_str());
}
