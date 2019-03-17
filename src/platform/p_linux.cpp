#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "platform/p_linux.h"
#include "define/define.h"
#include <locale.h>

/**< 获得服务器开启毫秒数， 大于 49天后，重置为 1 */
uint64_t GetTickCount64_Q()
{
    int64_t  currentTime      = 0;
    static int64_t  loctv_sec = 0;
    static int64_t  loctv_usec= 0;

    if (loctv_sec <= 0 && loctv_usec <= 0)
    {
        struct timeval current1;
        gettimeofday(&current1, NULL);
        loctv_sec  = current1.tv_sec;
        loctv_usec = current1.tv_usec;
    }

    struct timeval current;
    gettimeofday(&current, NULL);
    currentTime = ((current.tv_sec - loctv_sec) * 1000 + (current.tv_usec - loctv_usec) / 1000) + 100;

    return currentTime;
}
uint32_t GetTickCount32_Q()
{
    uint32_t  currentTime = 0;
    static int64_t  loctv_sec = 0;
    static int64_t  loctv_usec = 0;

    if(loctv_sec <= 0 && loctv_usec <= 0)
    {
        struct timeval current1;
        gettimeofday(&current1, NULL);
        loctv_sec  = current1.tv_sec;
        loctv_usec = current1.tv_usec;
    }

    struct timeval current;
    gettimeofday(&current, NULL);
    currentTime = ((current.tv_sec - loctv_sec) * 1000 + (current.tv_usec - loctv_usec)/1000) % 4290000000 + 100;

    return currentTime;
}
uint32_t timeGetTime_Q()
{
    uint32_t uptime = 0;
    struct timespec on;
    if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
    {
        uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
    }
    return uptime;
}
#include <sys/sysinfo.h>
uint32_t GetUptime()
{
    struct sysinfo s_info;

    sysinfo(&s_info);

    return s_info.uptime;
}

struct tm *  localtime_q(tm *ptm, const time_t *ptime)
{
    return localtime_r(ptime,ptm);
}

//-------------------------------"xxx_s" function------------------------------------------------
void    strcpy_q(char * dest,size_t dsize,const char * source)
{
    if (dest && source)
    {
        size_t nMin = safe_min(dsize, strlen(source));
        strncpy(dest, source, nMin);
        dest[safe_min(nMin, dsize - 1)] = '\0';
    }
    return;
}

void    strncpy_q(char * dest,const char * source,size_t dsize)
{
    strcpy_q(dest, dsize, source);
}

int32_t stricmp_q(const char *s1, const char *s2)
{
    return strcasecmp(s1,s2);
}

int32_t strnicmp_q(const char *s1, const char *s2,size_t dsize)
{
    return strncasecmp(s1,s2,dsize);
}

void    memcpy_q(void * dest, size_t dsize, const void * source, size_t szsource)
{
    if (dest && source)
    {
        size_t nMin = safe_min(dsize, szsource);
        memcpy(dest, source, nMin);
    }
    return;
}

int32_t vsprintf_q(char *str, size_t num, const char *format, va_list valist)
{
    return vsnprintf(str, num, format, valist);
}

void    fopen_q(FILE **fh, const char* filename, const char* open)
{
    (*fh) = fopen(filename, open);
}
//-------------------------------system function------------------------------------------------
uint64_t GetCurrentProcessId_Q()
{
    pid_t dwPid = 0;
    dwPid = getpid();
    return static_cast<uint64_t>(dwPid);
}
uint64_t GetCurrentThreadId_Q()
{
    pthread_t dwThreadid = 0;
    dwThreadid = pthread_self();
    return static_cast<pthread_t>(dwThreadid);
}

int32_t GetModuleFileName_Q( char* sModuleName, char* sFileName, int32_t nSize)
{
    char _exeName[] = "/proc/self/exe";

    size_t linksize = 256;
    char exeName[256] = { 0 };

    int ret = readlink(_exeName, exeName, linksize);

    if (ret != -1)
    {
        char *tmp = strrchr(exeName, '/');

        if (tmp != NULL)
        {
            strcpy_q(sFileName, nSize, tmp + 1);
        }
    }
    return ret;
}

bool CreateDirectory_Q(const char* szPath,void *p)
{
    mode_t  mode=0777;
    if(mkdir(szPath,mode)==0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool IsDirOrFileExist(const char * szDir)
{
    if (access(szDir, 0) == 0)
    {
        return true;
    }
    return false;
}

void Sleep_Q(uint32_t dwMilliseconds)
{
    usleep(dwMilliseconds*1000);
}

int64_t strto64_q(const char *nptr, char **endptr, int base)
{
    return strtoll(nptr,endptr,base);
}

uint64_t strtou64_q(const char *nptr, char **endptr, int base)
{
	return strtoull(nptr,endptr,base);
}

char *  ui64toa_q(uint64_t value, char *str, int radix)
{
    char buffer[65];
    char *pos;

    pos = &buffer[64];
    *pos = '\0';

    do
    {
		int digit = value % radix;
        value = value / radix;
        if (digit < 10)
        {
            *--pos = '0' + digit;
        }
        else
        {
            *--pos = 'a' + digit - 10;
        } /* if */
    }
    while (value != 0L);

    memcpy(str, pos, &buffer[64] - pos + 1);
    return str;
}

int32_t mb2wc_q( char *src,wchar_t *des,size_t desMaxLen )
{
	if (src==NULL||des==NULL||desMaxLen==0)
	{
		return 0;
	}

	if (NULL == setlocale(LC_ALL, "zh_CN.UTF-8"))//设置转换为unicode前的码,当前为gbk编码
	{
		return 0;
	}

	//首先先将gbk编码转换为unicode编码
	int32_t unicodeLen=mbstowcs(NULL,src,0);//计算转换后的长度     gbk->unicode
	int32_t nAllBytes=unicodeLen*sizeof(wchar_t)+sizeof(wchar_t);
	if(unicodeLen<=0||nAllBytes>(int32_t)desMaxLen)
	{
		return 0;
	}
	mbstowcs(des,src,strlen(src));//将gbk转换为unicode
	des[unicodeLen]=0;
	return nAllBytes;//strlen + 结尾长度
}
int32_t wc2mb_q( wchar_t* src,char* des,size_t desMaxLen )
{
	if (src==NULL||des==NULL||desMaxLen==0)
	{
		return 0;
	}
	//将unicode编码转换为utf8编码
	if(NULL==setlocale(LC_ALL,"zh_CN.UTF-8"))//unicode->gbk
	{
		return 0;
	}
	int32_t utfLen=wcstombs(NULL,src,0);//计算转换后的长度
	int32_t nAllBytes=utfLen*sizeof(char)+sizeof(char);
	if(utfLen<=0||nAllBytes>desMaxLen)
	{
		return 0;
	}

	wcstombs(des,src,utfLen);
	des[utfLen]=0;//添加结束符
	return nAllBytes;//返回strlen + 结尾长度
}










