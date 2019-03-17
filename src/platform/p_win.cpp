
#include "platform/p_win.h"
#include  <mmsystem.h>//for timeGetTime;
#include <string.h>
#include <time.h>
#include <io.h>
#include "define/define.h"
#include "platform/platform.h"


//---------------------------time function----------------------------------------------------
uint64_t GetTickCount64_Q()
{
    return GetTickCount64();
}
uint32_t GetTickCount32_Q()
{
    return GetTickCount();
}
uint32_t timeGetTime_Q()
{
    return timeGetTime();
}
uint32_t GetUptime()
{
    return timeGetTime()/1000;
}

struct tm *  localtime_q(tm *ptm, const time_t *ptime)
{
    localtime_s(ptm,ptime);
    return ptm;
}

//-------------------------------"xxx_s" function------------------------------------------------
void    strcpy_q(char * dest,size_t dsize,const char * source)
{
    if (dest && source)
    {
        size_t nMin = safe_min(dsize, strlen(source));
        //strncpy(dest, source, nMin);
        memcpy(dest,source,nMin);
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
    return _stricmp(s1,s2);
}
int32_t	strnicmp_q(const char *s1, const char *s2,size_t dsize)
{
	return _strnicmp(s1,s2,dsize);
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

int32_t vsprintf_q(char *str, size_t num, const char *format, va_list ap)
{
    return _vsnprintf(str, num, format, ap);
}

void fopen_q(FILE **fh, const char* filename, const char* open)
{
    fopen_s(fh,filename,open);
}
//-------------------------------system function------------------------------------------------
uint64_t GetCurrentProcessId_Q()
{
    return GetCurrentProcessId();
}
uint64_t GetCurrentThreadId_Q()
{
    return GetCurrentThreadId();
}

int32_t  GetModuleFileName_Q(char* sModuleName, char* sFileName, int32_t nSize)
{
    return GetModuleFileNameA(NULL,sFileName,(uint32_t)nSize);
}

bool CreateDirectory_Q(const char* szPath,void *p)
{
    LPSECURITY_ATTRIBUTES ptmp = (LPSECURITY_ATTRIBUTES)p;
    if (CreateDirectoryA(szPath,ptmp))
    {
        return true;
    }
    return false;
}
bool IsDirOrFileExist(const char * szDir)
{
    if (_access(szDir, 0) == 0)
    {
        return true;
    }
    return false;
}

void Sleep_Q(uint32_t dwMilliseconds)
{
    Sleep(dwMilliseconds);
}

int64_t strto64_q(const char *nptr, char **endptr, int base)
{
    return _strtoi64(nptr,endptr,base);
}
char *  ui64toa_q(uint64_t value, char *str, int radix)
{
    return _ui64toa(value,str,radix);
}

int32_t mb2wc_q( char *src,wchar_t *des,size_t desMaxLen )
{
	int32_t nRetLen = 0;
	do 
	{
		if(src == NULL || des == NULL || desMaxLen==0)break;  //字符串为NULL则出错退出
		nRetLen = ::MultiByteToWideChar(CP_UTF8,0,src,-1,NULL,NULL);  //获取转换后所需要的字符空间长度
		if(nRetLen == 0 || desMaxLen < nRetLen)break;//字符串空间不足
		nRetLen = ::MultiByteToWideChar(CP_UTF8,0,src,-1,des,desMaxLen);  //转换
	}while(false);
	return nRetLen;
}
int32_t wc2mb_q( wchar_t* src,char* des,size_t desMaxLen )
{
	int32_t nRetLen = 0;
	do 
	{
		if(src == NULL || des == NULL || desMaxLen==0)break;  //符串为NULL则出错退出
		nRetLen = ::WideCharToMultiByte(CP_UTF8,0,src,-1,NULL,0,NULL,NULL);  //获取转换后所需要的字符空间长度
		if(nRetLen == 0 || desMaxLen < nRetLen)break;  //字符串空间不足
		nRetLen = ::WideCharToMultiByte(CP_UTF8,0,src,-1,(char *)des,desMaxLen,NULL,NULL);  //转换
	}while(false);
	return nRetLen;
}