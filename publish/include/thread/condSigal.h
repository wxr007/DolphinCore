/*
 * 
 *
 *  Created on: 2015年11月9日
 *      Author: Administrator
 */

#ifndef _COND_SIGNAL_H__
#define _COND_SIGNAL_H__

#include "define/noncopyable.h"
#include "thread/qthr_sync.h"
#include "thread/lockObj.h"

//#define CCONDSIGAL_PUSHCLEAN(condobj) \
//    pthread_cleanup_push(CCondSigal::static_threadKillClean, &(condobj.getCIntLock()))
//
//#define CCONDSIGAL_POP pthread_cleanup_pop(0)

#define CCONDSIGAL_PUSHCLEAN(condobj)
#define CCONDSIGAL_POP

class CCondSigal
{
public:
    CCondSigal();
    ~CCondSigal();
    static void static_threadKillClean(void* cLock);//清除信号
    void waitEvent();                               //等待信号
    void timedwaitEvent(int millisec = 30);
    void setEvent();                                //设置信号
    inline CIntLock& getCIntLock()
    {
        return m_CLock;
    }
private:
#ifdef WIN32
    QCondition m_cond;
    CIntLock       m_CLock;
#else//Linux;
    pthread_cond_t m_cond;
    CIntLock       m_CLock;
#endif
};


#endif //_COND_SIGNAL_H__