#include"utils/convert_code/convEncode.h"
#include "platform/platform.h"

#ifdef WIN32

#define UNICODE_MAX_LEN 1024*1024	//分配1m空间
__thread wchar_t lpUnicodeStr[UNICODE_MAX_LEN] = {0}; //用线程变量防止多线程冲突

int GBKToUTF8(char * lpGBKStr,char * lpUTF8Str,int nUTF8StrLen)
{
	int nRetLen = 0;
	do 
	{
		if(lpGBKStr == NULL || lpUTF8Str == NULL)break;  //如果UTF8字符串或GBK字符串为NULL则出错退出
		nRetLen = ::MultiByteToWideChar(CP_ACP,0,lpGBKStr,-1,NULL,NULL);  //获取转换到Unicode编码后所需要的字符空间长度
		if(nRetLen == 0 || UNICODE_MAX_LEN < nRetLen)break;//Unicode字符串空间不足
		nRetLen = ::MultiByteToWideChar(CP_ACP,0,lpGBKStr,-1,lpUnicodeStr,nRetLen);  //转换到Unicode编码
		if(nRetLen == 0)break;  //转换失败则出错退出
		nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //获取转换到UTF8编码后所需要的字符空间长度
		if(nUTF8StrLen < nRetLen)break;  //如果输出缓冲区长度不够则退出
		nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,(char *)lpUTF8Str,nUTF8StrLen,NULL,NULL);  //转换到UTF8编码
	} while (false);
	return nRetLen;
}

int UTF8ToGBK(char * lpUTF8Str,char * lpGBKStr,int nGBKStrLen)
{
	int nRetLen = 0;
	do 
	{
		if(lpGBKStr == NULL || lpUTF8Str == NULL)break;  //如果UTF8字符串或GBK字符串为NULL则出错退出
		nRetLen = ::MultiByteToWideChar(CP_UTF8,0,lpUTF8Str,-1,NULL,NULL);  //获取转换到Unicode编码后所需要的字符空间长度
		if(nRetLen == 0 || UNICODE_MAX_LEN < nRetLen)break;//Unicode字符串空间不足
		nRetLen = ::MultiByteToWideChar(CP_UTF8,0,lpUTF8Str,-1,lpUnicodeStr,nRetLen);  //转换到Unicode编码
		if(nRetLen == 0)break;  //转换失败则出错退出
		nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,NULL,NULL,NULL,NULL);  //获取转换到GBK编码后所需要的字符空间长度
		if(nGBKStrLen < nRetLen)break;//如果输出缓冲区长度不够则退出
		nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nGBKStrLen,NULL,NULL);  //转换到GBK编码
	} while (false);
	return nRetLen;
}

#else

#include <iconv.h>
#include <string.h>//strlen

#define UTF8ENCODING    "UTF-8"
#define GBKENCODING		"GBK"

int ConvEnCoding(const char * szFrom ,const char * szTo,char * src,char* des,size_t nMaxBufferLen)
{
    if (szFrom==NULL||szTo==NULL||src==NULL)
    {
        return -1;
    }
    char* pDes=des;//注意 这里的des会被 转换的时候写入新 地址  所以先保存
    size_t srcLen=strlen(src);
    size_t nMaxLen=nMaxBufferLen;
    iconv_t cd;
    int nret=0;
    cd=iconv_open(szTo,szFrom);
    if (cd==(void*)-1)
    {
        return -1;
    }
    iconv(cd,&src,&srcLen,&des,&nMaxBufferLen);
    iconv_close(cd);
    nret=nMaxLen-nMaxBufferLen;
    pDes[nret]='\0';
    return nret;
}

int UTF8ToGBK(char * lpUTF8Str,char * lpGBKStr,int nGBKStrLen)
{
	return ConvEnCoding(UTF8ENCODING, GBKENCODING, lpUTF8Str, lpGBKStr, nGBKStrLen);
}

int GBKToUTF8(char * lpGBKStr,char * lpUTF8Str,int nUTF8StrLen)
{
	return ConvEnCoding(GBKENCODING, UTF8ENCODING, lpGBKStr, lpUTF8Str, nUTF8StrLen);
}

#endif