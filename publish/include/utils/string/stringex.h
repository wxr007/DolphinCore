#ifndef _STRINGEX_H__
#define _STRINGEX_H__

#include "platform/platform.h"

#include<string.h>
#include<time.h>
// #include<unistd.h>
#include<string>
// #include<bits/stl_function.h>
#include<wctype.h>

using namespace std;

#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define ZeroMemory RtlZeroMemory
#define COUNT_OF(X) (sizeof(X)/sizeof((X)[0]))
#define _TH_VAR_COUNT_OF(V)			COUNT_OF(V)

#define _TH_VAR(T,V) __thread T V;
#define _TH_VAR_INIT(T,V,code) __thread T V;

#define _TH_VAR_ARRAY(T,V,count)  __thread T V[count];//线程局部存储机制 保证一个线程拥有不同的副本对象
#define _TH_VAR_INIT_ARRAY(T,V,count,initobj)  __thread T V[count]={ initobj };

#define _TH_VAR_GET(V)				V
#define _TH_VAR_PTR(V)				&V
#define _TH_VAR_SET(V,A)			V=(A);
#define _TH_VAR_SET_PTR(A)
#define _TH_VAR_INDEX(V)			0
#define _TH_VAR_SIZEOF(V)			sizeof(V)
#define _TH_VAR_COUNT_OF(V)			COUNT_OF(V)
#define _TH_VAR_FREE

#define MAX_PATH          260
#define _TH_VAR_PTR(V)				&V
#define MAX_TLS_LOOPCHARBUFFER				1024*128
extern _TH_VAR_ARRAY(char, tls_loop_charbuffer, MAX_TLS_LOOPCHARBUFFER + 1);

#define _GET_TLS_LOOPCHARBUF(x)				char* ptlsbuf=(char*)_TH_VAR_PTR(tls_loop_charbuffer);		\
											char** ptlslast= (char**)(ptlsbuf);		\
											ptlsbuf+=sizeof(char**);		\
											if (!(*ptlslast>=ptlsbuf && *ptlslast<=(ptlsbuf+MAX_TLS_LOOPCHARBUFFER-8)))	\
											{*ptlslast=ptlsbuf;}		\
											size_t ntlslen=strlen(*ptlslast)+(*ptlslast-ptlsbuf);	\
											if (ntlslen>(MAX_TLS_LOOPCHARBUFFER-1024-128) || ((ntlslen+(x))>(MAX_TLS_LOOPCHARBUFFER-32)) ){		\
											ntlslen=_TH_VAR_COUNT_OF(tls_loop_charbuffer)-sizeof(char**);\
											ZeroMemory(ptlsbuf,MAX_PATH);*ptlslast=(ptlsbuf+4);ntlslen -= 16;}\
											else {*ptlslast=ptlsbuf+ntlslen+4;		\
											ntlslen=_TH_VAR_COUNT_OF(tls_loop_charbuffer)-ntlslen-16;}	\
											ptlsbuf=*ptlslast;	ptlsbuf[0]=0;	\

#define _GET_TH_LOOPCHARBUF(x,y)  _GET_TLS_LOOPCHARBUF(x)

void replacelashPath(char *str, bool front = true);
void str_trim_right(char* psz);
void str_trim_left(char* psz);
void str_trim(char* psz);

#define  replaceBacklashPath(x)		replacelashPath(x,false)
#define  replaceFrontlashPath(x)	replacelashPath(x,true)

char* timetostr(time_t time1, char *szTime = NULL, int nLen = 0, const char* sformat = "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d");
const char* extractfilepath(const char* pname);

const char* vformat(const char* pszFmt, ...);

#ifdef WIN32
#include <hash_map>
#else
#include<ext/hash_map>
using namespace __gnu_cxx;
#endif
#include<string>
using namespace std;
/*************************************************************/

struct string_key_hash : public unary_function<const string, size_t>
{
	size_t operator()(const std::string &x) const;
};

struct string_key_case_hash : public std::unary_function<const std::string, size_t>
{
	size_t operator()(const std::string &x) const;
};

struct string_key_equal : public std::binary_function<const std::string, const std::string, bool>
{
	bool operator()(const std::string &s1, const std::string &s2) const;
};

struct string_key_case_equal : public std::binary_function<const std::string, const std::string, bool>
{
	bool operator()(const std::string &s1, const std::string &s2) const;
};



struct pchar_key_hash : public std::unary_function<const char*, size_t>
{
	size_t operator()(const char* x) const;
};

struct pchar_key_case_hash : public std::unary_function<const char*, size_t>
{
	size_t operator()(const char* x) const;
};

struct pchar_key_equal : public std::binary_function<const char*, const char*, bool>
{
	bool operator()(const char* s1, const char* s2) const;
};

struct pchar_key_case_equal : public std::binary_function<const char*, const char*, bool>
{
	bool operator()(const char* s1, const char* s2) const;
};


int Mem2Hex(char* pin, int nsize, char* pout, int nout);
int AbsMem2Hex(char* pin, int nsize, int nbef, int nmid, int naft, char* pout, int nout);

#define __BUILD_DATE_TIME__			__ZDATE__
#define __ZDATE__	endate2date(__DATE__)

char* endate2date(char* src);
const char* sec2str(int iSecond);

//=========================================================
const char* extractfiletitle(const char* pname);
const char* extractfilename(const char* pname);                         //Get Progarm Name
const char* changefileext(const char* pname,const char* pchg);
void str_replace(const char* pszSrc, char* pszDst, int ndlen , const char* pszOld, const char* pszNew);
void str_replace(char* pszSrc, const char* pszOld, const char* pszNew);
void str_replace(char* pszSrc, char cOld, char cNew);
void str_replace(char* pszSrc, wchar_t cOld, wchar_t cNew);
std::string str_Replace(std::string &strText, const char *pszSrcText, const char *pszDestText);


#endif//_STRINGEX_H__
