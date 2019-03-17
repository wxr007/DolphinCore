#ifndef _PLATFORM_H__
#define _PLATFORM_H__

//#include "uv.h"

#ifdef WIN32
#include "platform/p_win.h"
#else
#include "platform/p_linux.h"
#endif

//函数声明;
#include <stdint.h>
#include <stdio.h>
#include <time.h>//localtime_q
#include <ctype.h>
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++平台相关的公用函数,在p_win/p_linux.cpp中实现++++++++++++++++++++++++++++++++++
//1-获取开机到现在的时间,单位是ms(毫秒).
uint64_t GetTickCount64_Q();//replace '_64GetTickCount';
uint32_t GetTickCount32_Q();//replace 'GetTickCount';
uint32_t timeGetTime_Q();//注意,该函数GetTickCount32_Q类似;但是，该函数精度不高,性能消耗低.一般，该函数够用.
struct tm *  localtime_q(tm *ptm, const time_t *ptime);//should include "time.h"
uint32_t GetUptime();//返回开机到现在持续时间（秒数）.
//uint32_t GetWeiMiaoCount(void);
//这两个函数直接删除,可以用std::clock();//time.h
//bool QueryPerformanceFrequency(LARGE_INTEGER*counter);//clock_getres(CLOCK_MONOTONIC,...)
//bool QueryPerformanceCounter(LARGE_INTEGER *counter);//lock_gettime(CLOCK_MONOTONIC,...)


//2-1-多字节转宽字节;
//int32_t MultiByteToWideChar_Q(char *src, wchar_t *des, size_t desMaxLen);//gbk to unicode wchar_t 注意windows wchar_t 2bytes  linux wchar_t 4bytes
//2-2-宽字节转多字节;
//int32_t WideCharToMultiByte_Q(wchar_t* src, char* des, size_t desMaxLen);//unicode to gbk

//3-windows 'xxx_s'函数替换.
void    strcpy_q(char * dest,size_t dsize,const char * source);//取长度最小,copy.
void    strncpy_q(char * dest,const char * source,size_t dsize);//取长度最小,copy.
int32_t stricmp_q(const char *s1, const char *s2);//判断字符串是否相等忽略大小写
int32_t	strnicmp_q(const char *s1, const char *s2,size_t dsize);
void    memcpy_q(void * dest, size_t dsize, const void * source, size_t szsource);
int32_t vsprintf_q(char *str, size_t num, const char *format, va_list valist);
// int32_t vsnprintf_q(char* src, size_t maxlen, const char* format, va_list arg);
void    fopen_q(FILE **fh, const char* filename, const char* open);

uint64_t GetCurrentProcessId_Q();//获得当前进程id
uint64_t GetCurrentThreadId_Q();//获得当前线程id
int32_t  GetModuleFileName_Q(char* sModuleName, char* sFileName, int32_t nSize);	//获得当前执行文件的路径
bool CreateDirectory_Q(const char* szPath,void *p);//创建目录
bool IsDirOrFileExist(const char * szDir);//文件活动目录是否存在

void Sleep_Q(unsigned int dwMilliseconds);//实现毫秒级休息

int64_t strto64_q(const char *nptr, char **endptr, int base);
uint64_t strtou64_q(const char *nptr, char **endptr, int base);
char *  ui64toa_q(uint64_t value, char *str, int radix);//64 int  to string

void syslog_q(const char *log);

//这两个函数是用于init文件的,现在没什么用了.
//uint32_t GetPrivateProfileStringA(char* ptzSectionName, char* ptzKeyName, char* ptzDefault, char* ptzReturnedString, uint32_t dwSize, char* szFilename, bool bforceupdate = false);
//bool WritePrivateProfileStringA(char* ptzSectionName, char* ptzKeyName, char* szdata, char* szfilename);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++平台无关的公用函数,在com_func.cpp中实现++++++++++++++++++++++++++++++++++
//全部转换成小写,修改原数据内容.
char*  strlwr_q(char* str);

uint32_t Random_Q(uint32_t nMax,uint32_t nMin = 0);
void random_full(void* pbuf, int nsize);

time_t strtotime_q(const char * szTime, const char* sformat = "%4d-%2d-%2d %2d:%2d:%2d");

// void FilterFirSpaceAndEndTab(const char*szIn, char*szOut);
//
// void FileterFirAndEndQuotes(const char*szIn, char* szOut, uint32_t dwsize);
int32_t sprintf_q(char *str, size_t num, const char *format, ...);
bool  _safe_vsnprintf(char *string, size_t count, const char *format, va_list ap);

char* md5to32char(const char*szSrc);


int32_t InterlockedIncrement_Q(int32_t* lpAddend );//线程安全自加1
int32_t InterlockedDecrement_Q(int32_t* lpAddend);//线程安全自减1
int32_t SafeIncrement(volatile int32_t& nNum);//线程安全自加1
int32_t SafeDecrement(volatile int32_t& nNum);//线程安全自减1
int32_t SafeGet(volatile int32_t& nNum);//线程安全获取
void SafeSet(volatile int32_t& nNum, int32_t value);//线程安全赋值

int32_t mb2wc_q(char *src,wchar_t *des,size_t desMaxLen);//utf-8多字节转宽字节
int32_t wc2mb_q(wchar_t* src,char* des,size_t desMaxLen);//utf-8宽字节转多字节

//[ini] api  返回copy的字节数[在CSimpleIniManage.cpp实现]
uint32_t GetPrivateProfileStringA(char* ptzSectionName, char* ptzKeyName, char* ptzDefault, char* ptzReturnedString, uint32_t dwSize, char* szFilename, bool bforceupdate = false);
bool WritePrivateProfileStringA(char* ptzSectionName, char* ptzKeyName, char* szdata, char* szfilename);

#endif //_PLATFORM_H__
