/*
 * atomic.h
 *
 *  Created on: 2015年10月10日
 *      Author: Administrator
 */

#ifndef _ATOMIC_H__
#define _ATOMIC_H__

#ifdef _USE_CXX11_

#include <atomic>

namespace QCOOL
{
using std::atomic;
}

#else//not def _USE_CXX11_;
//using boost;
// #include <boost/atomic.hpp>
// 
// namespace QCOOL
// {
// using boost::atomic;
// }
//由于CENTOS6.5的boost版本过低,所以无法用boost::atomic;
#include "thread/qthr_sync.h"
namespace QCOOL
{
    template<typename T>
    class atomic
    {
    public:
        atomic(T v = 0) :v_(v){}
        T fetch_add(T diff)
        {
            QMutexGuard guard(mutex_);
            v_ += diff;
            return v_;
        }
    private:
        QMutex mutex_;
        T v_;
    };
}
#endif

#endif //_ATOMIC_H__
