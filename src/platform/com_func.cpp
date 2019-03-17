
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "encrypt/md5/md5ex.h"
#include "platform/platform.h"

char*  strlwr_q(char* str)
{
	while (*str)
	{
		*str = tolower(*str);
		++str;
	}
	return str;
}

time_t strtotime_q(const char * szTime, const char* sformat)
{
    struct tm tm1;
    memset(&tm1, 0, sizeof(tm1));
    sscanf(szTime, sformat,&tm1.tm_year,&tm1.tm_mon,&tm1.tm_mday,&tm1.tm_hour,&tm1.tm_min,&tm1.tm_sec);
    tm1.tm_year -= 1900;
    tm1.tm_mon--;
    tm1.tm_isdst = -1;
    return mktime(&tm1);
}

int32_t sprintf_q(char *str, size_t num, const char *format, ...)
{
    va_list ap;
    int r = 0;
    va_start(ap, format);
    r = vsprintf_q(str, num, format, ap);
    va_end(ap);
    return r;
}

bool  _safe_vsnprintf(char *string, size_t count, const char *format, va_list ap)
{
    vsprintf_q(string,count, format, ap);
    return true;
}

static void RandomInit()
{
    static bool static_bosrand = false;
    if (!static_bosrand)
    {
        srand(timeGetTime_Q());
        static_bosrand = true;
    }
}


uint32_t Random_Q(uint32_t nMax, uint32_t nMin)
{
    RandomInit();
    if (nMax > nMin)
    {
        int nr1 = ((rand() << 16) | rand());
        int nr2 = ((rand() << 16) | rand());
        unsigned int nmod = (nMax - nMin);
        unsigned int nr = (abs(nr1 - nr2));
        return nmod == 0 ? nr : ((nr % nmod) + nMin);
    }
    else
    {
        return nMin;
    }
}

void random_full(void* pbuf, int nsize)
{
    for(int i = 0; i < (nsize / 4); i++)
    {
        *((uint32_t*)pbuf) = (uint32_t)Random_Q(0xffffffff, 0);
        pbuf = ((char*)pbuf) + 4;
    }

    int nmod = nsize % 4;

    if(nmod > 0)
    {
        uint32_t nrand = (uint32_t)Random_Q(0xffffffff, 0);
        memcpy(pbuf, &nrand, nmod);
    }
}

char* md5to32char(const char*szSrc)
{
    if (szSrc == NULL)
    {
        return NULL;
    }
    MD5_DIGEST md5;
    MD5String(szSrc, &md5);
    char szTemp[48];
    static char m_szNewCreature[48];
    memset(m_szNewCreature, 0, 48);
    for (int i = 0; i < 16; i++)
    {
        sprintf(szTemp, "%.2x", md5[i]);//转换成可用的 md5
        strcat(m_szNewCreature, szTemp);
    }
    return m_szNewCreature;
}

//-------------------------------atomic function------------------------------------------------
#include "thread/lockObj.h"
static CMutexLock g_atomic_;
int32_t InterlockedIncrement_Q(int32_t* lpAddend )
{
    CMutexAutoLock locker(g_atomic_);
    return ++(*lpAddend);
}
int32_t InterlockedDecrement_Q(int32_t* lpAddend)
{
    CMutexAutoLock locker(g_atomic_);
    return --(*lpAddend);
}

int32_t SafeIncrement(volatile int32_t& nNum)
{
    CMutexAutoLock locker(g_atomic_);
    return ++nNum;
}
int32_t SafeDecrement(volatile int32_t& nNum)
{
    CMutexAutoLock locker(g_atomic_);
    return --nNum;
}
int32_t SafeGet(volatile int32_t& nNum)
{
    CMutexAutoLock locker(g_atomic_);
    return nNum;
}
void SafeSet(volatile int32_t& nNum, int32_t value)
{
    CMutexAutoLock locker(g_atomic_);
    nNum = value;
}
