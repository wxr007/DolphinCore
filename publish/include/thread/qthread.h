

#ifndef _QTHREAD_H__
#define _QTHREAD_H__

#include <string>
#include <uv.h>
//#include <functional>
#include "utils/function.h"

#include "thread/qthr_sync.h"
typedef QCOOL::function<void()> ThrFunc;

class QThread
{
public :
    QThread(ThrFunc func,std::string thrname="");

    ~QThread();

    bool Start();

    int32_t Join();

    int32_t CurrentThreadIdx() const ;
private :

    static void thread_start(void *arg);

private :
    QMutex mutex_;
    struct ThreadData
    {
        ThrFunc thr_func_;
        std::string thr_name_;
        struct ThreadInfo *thread_ptr_;
    };
    bool is_joined_;
    ThreadData thr_data_;
    struct ThreadInfo *thread_ptr_;

};







#endif //_QTHREAD_H__












