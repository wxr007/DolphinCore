/*
 * CCondSigal.cpp
 *
 *  Created on: 2015年11月9日
 *      Author: Administrator
 */

#include "thread/condSigal.h" 

#ifdef WIN32
CCondSigal::CCondSigal():m_cond(m_CLock.mutex_)
{
}

CCondSigal::~CCondSigal()
{
}

void CCondSigal::waitEvent()
{
    m_cond.Wait();
}

void CCondSigal::timedwaitEvent(int millisec)
{
    //1-get time;
    if (millisec < 0)
    {
        millisec = 30;
    }
    m_cond.TimeWait(millisec);
}

void CCondSigal::setEvent()
{
    m_cond.Notify();
}
#else//Linux;
#include <sys/time.h>

CCondSigal::CCondSigal()
{
    pthread_cond_init(&m_cond,NULL);
}

CCondSigal::~CCondSigal()
{
    pthread_cond_destroy(&m_cond);
}

void CCondSigal::static_threadKillClean(void * cLock)
{
    if(cLock)
    {
        CIntLock* pLock=(CIntLock*)cLock;
        pLock->Unlock();
    }
    return;
}


void CCondSigal::waitEvent()
{
    pthread_cond_wait(&m_cond, &(m_CLock.getMutex()));
}

void CCondSigal::timedwaitEvent(int millisec)
{
    //1-get time;
    struct timeval temp;
    gettimeofday(&temp, 0);
    if (millisec < 0)
    {
        millisec = 30;
    }
    temp.tv_usec = temp.tv_usec + millisec * 1000;
    if (temp.tv_usec > 1000000)
    {
        temp.tv_sec  += temp.tv_usec / 1000000;
        temp.tv_usec %= 1000000;
    }

    //2-set time;
    struct timespec abstime;
    abstime.tv_sec  = temp.tv_sec;
    abstime.tv_nsec = (temp.tv_usec) * 1000;
    pthread_cond_timedwait(&m_cond, &(m_CLock.getMutex()), &abstime);	
}

void CCondSigal::setEvent()
{
    pthread_cond_signal(&m_cond);
}
#endif