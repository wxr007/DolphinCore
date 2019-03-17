
#ifndef _LOCK_OBJ_H__
#define _LOCK_OBJ_H__

#include "thread/qthr_sync.h"
#include"define/noncopyable.h"

#ifdef WIN32
class CMutexLock
{
public :
    void Lock()
    {
        mutex_.Lock();
    }
    void Unlock()
    {
        mutex_.UnLock();
    }
private :
    friend class CCondSigal;
    QMutex mutex_;
};
#else//Linux

#include <pthread.h>
#include <assert.h>
class CMutexLock
{
public:
    CMutexLock()
    {
        int nret1,nret2;
        nret1=pthread_mutexattr_init (&m_attr);
        pthread_mutexattr_settype (&m_attr, PTHREAD_MUTEX_RECURSIVE_NP);
        nret2= pthread_mutex_init (&m_mutex, &m_attr);
        if (nret1!=0||nret2!=0)
        {
            //perror("init mutex error\n");
            assert(0);
        }
    }
    ~CMutexLock()
    {
        int nret1= pthread_mutex_destroy(&m_mutex);
        int nret2= pthread_mutexattr_destroy(&m_attr);
        if (nret1!=0||nret2!=0)
        {
            //perror("Uninit mutex error\n");
            assert(0);
        }
    }
    void Lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    void Unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
    __inline pthread_mutex_t& getMutex()
    {
        return m_mutex;
    }
private:
    pthread_mutex_t m_mutex; //互斥对象
    pthread_mutexattr_t m_attr;//属性对象
};

#endif

class CMutexAutoLock : private zNoncopyable
{
public:
    CMutexAutoLock(CMutexLock& obj)//实际传递的是CMutexLock 对象
        : m_unlockobj(obj)
    {
        obj.Lock();
    }
    ~CMutexAutoLock()
    {
        m_unlockobj.Unlock();
    }
private:
    CMutexLock& m_unlockobj;
};

#define INFOLOCK(a)         (a).Lock()
#define UNINFOLOCK(a)       (a).Unlock()

typedef CMutexLock CIntLock;
typedef CMutexLock CInpLock;
//typedef CMutexBase stLockTNone;
typedef CMutexLock stLockTNone;

#define AILOCKT(a)      CMutexAutoLock tAutoInfoIntLock((a))

#endif //end _LOCK_OBJ_H__