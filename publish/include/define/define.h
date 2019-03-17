
#ifndef _DEFINE_H__
#define _DEFINE_H__



#include<string.h>
#include<sys/types.h>
#include "platform/platform.h"



#define MAXLONG     0x7fffffff
#define _STACK_AUTOEXEC_(name,initcode,uninitcode)		struct name{ name(){ initcode;}; ~name(){ uninitcode;}; };
#define _MAX_LIMITSEND_SIZE	1024*1024*5

struct stFuckTest
{
    void *p;
    bool bfree;
    char szName[500];
    char szAddName[500];
    char szDelName[500];
};

#include<map>
using namespace std;
extern map<void* ,stFuckTest> g_mpNewData;
extern map<void* ,stFuckTest> g_mpDefData;



void deleOne(void*p,const char* szName,const char* szFile,int nLine,const char* szFuc);
void addOne(void*p,const char* szName,const char* szFile,int nLine,const char* szFuc);
void showAll();

#define DEFINE_ADD_ONE(a)// addOne(a,#a,__FILE__,__LINE__,__FUNCTION__);

#define DEFINE_DEL_ONE(a) //deleOne(a,#a,__FILE__,__LINE__,__FUNCTION__);




#define FUNCTION_BEGIN  //g_logger.debug("file:%s line:%d func:%s",__FILE__,__LINE__,__FUNCTION__);
#define STATIC_ASSERTMN2(p1,p2,p3,p4)
#define ROUNDNUM2(value,num)		( ((value) + ((num)-1)) & (~((num)-1)) )



#define SAFE_DELETE(x) { if (x) { delete (x); (x) = NULL; } }
#define SAFE_DELETE_VEC(x) { if (x) { delete [] (x);(x) = NULL; } }


#define SAFE_DELETEARRAY(x)		SAFE_DELETE_VEC(x)
#define SAFE_RELEASE(p)  if(p) { (p)->Release(); (p) = NULL; }
#define SAFE_EXEC_NULL(p,code)	if(p) { code; (p) = NULL; }
#define COUNT_OF(X) (sizeof(X)/sizeof((X)[0]))
#define count_of(X) COUNT_OF(X)
#define MAKEBIT(x) (1<<(x))
#define BIT(X) MAKEBIT(X)

//直接用UINT32_PTR替换DWORD_PTR
//#define DWORD_PTR unsigned int
#define UINT32_PTR uint32_t

//替换MAKEWORD
#define MAKEINT16(a, b)      ((uint16_t)(((uint8_t)(((UINT32_PTR)(a)) & 0xff)) | ((uint16_t)((uint8_t)(((UINT32_PTR)(b)) & 0xff))) << 8))
//替换MAKELONG
#define MAKEINT32(a, b)      ((int32_t)(((uint16_t)(((UINT32_PTR)(a)) & 0xffff)) | ((uint32_t)((uint16_t)(((UINT32_PTR)(b)) & 0xffff))) << 16))


#define _GATEWAY_CONN_ 0


#define GETCURREIP(x)	uint32_t x=NULL; {	 \
__asm push eax			\
__asm call __curreipx		\
__asm __curreipx:		\
__asm pop eax		\
__asm mov x,eax		\
__asm pop eax }


#define _STD_BEGIN	namespace std {
#define _STD_END		}

#define ROUNDNUM2(value,num)		( ((value) + ((num)-1)) & (~((num)-1)) )
#define MAX_PATH          260


template<typename T>
class Typedef2BASE
{
public:
	typedef T BASE;
};

template <typename _DT1, typename _DT2> class stlink2
{
public:
	_DT1 _p1;
	_DT2 _p2;
};
template <typename _DT1, typename _DT2, typename _DT3> class stlink3
{
	_DT1 _p1;
	_DT2 _p2;
	_DT3 _p3;
};
template <typename _DT1, typename _DT2, typename _DT3, typename _DT4> class stlink4
{
	_DT1 _p1;
	_DT2 _p2;
	_DT3 _p3;
	_DT4 _p4;
};
template <typename _DT1, typename _DT2, typename _DT3, typename _DT4, typename _DT5> class stlink5
{
	_DT1 _p1;
	_DT2 _p2;
	_DT3 _p3;
	_DT4 _p4;
	_DT5 _p5;
};



template<class _Ty> inline
const _Ty& safe_min(const _Ty& _Left, const _Ty& _Right){
	return (_Right < _Left ? _Right : _Left);
}

template<class _Ty> inline
const _Ty& safe_max(const _Ty& _Left, const _Ty& _Right){
	return (_Left < _Right ? _Right : _Left);
}

typedef unsigned char       uint8_t;


//缺少的
//typedef signed short SWORD;


#define NULL0			'\0'		
#define TOSTR(x)  #x
#define TOSTR1(x)      TOSTR(x)
#define __FUNC_LINE__	"("TOSTR1(__LINE__)"):"
#define __FILE_LINE__	__FILE__"("TOSTR1(__LINE__)")"
#define __FILE_FUNC__	__FILE__":"__FUNCTION__
#define __FF_LINE__		__FILE__"("TOSTR1(__LINE__)"):"__FUNCTION__
#define __FF_LINE_T__	__FILE__"("TOSTR1(__LINE__)")["__TIMESTAMP__"]:"__FUNCTION__

#define SOCKADDR struct sockaddr

//#define INVALID_SOCKET -1 //

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)

//#define __try       try
//#define __except    catch
//#define EXCEPTION_EXECUTE_HANDLER ...

//WARN
//#define FIONREAD 0 //假设为0
//#define WSAEWOULDBLOCK EWOULDBLOCK //假设为0

//#define SOCKET_ERROR -1

//INFO memory operate

#define RtlEqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

#define MoveMemory RtlMoveMemory
#define CopyMemory RtlCopyMemory
#define FillMemory RtlFillMemory
#define ZeroMemory RtlZeroMemory


#define ZEROSELF  ZeroMemory(this,sizeof(*(this)));
//typedef unsigned int size_t;

//




struct stAutoAlloc
{
	char* m_p;
	stAutoAlloc()
	{
		m_p = NULL;
	}
	~stAutoAlloc()
	{
	    DEFINE_DEL_ONE(m_p);
		SAFE_DELETE_VEC(m_p);
	}
};

#define STACK_ALLOCA(t,p,n)		t p=NULL;size_t sas##p=(safe_max< size_t>(n,16)*sizeof(p[0]));stAutoAlloc aac##p;if (sas##p>32*1024)	\
{p=(t)(new char[sas##p]);DEFINE_ADD_ONE(p);aac##p.m_p=(char*)p;*((uint32_t*)p)=0; }else{p=(t)(new char[sas##p]);aac##p.m_p=(char*)p;*((uint32_t*)p)=0;};\

#define ZSTACK_ALLOCA(t,p,n)		STACK_ALLOCA(t,p,n);if(p){ZeroMemory(p,sas##p);};


/*--->[ 定长数据包检测 ]*/
#define _CHECK_PACKAGE_LEN(st,cmdlen){\
	if (sizeof(st)!=cmdlen){\
	g_logger.debug("收到包 %s 接收长度 %d != 定义长度 %d ", #st,cmdlen,sizeof(st));\
	return false;}\
	}

/*--->[ 变长数据包, 检测接收的数据包长度是否小于需要的数据包长度 ]*/
#define _CHECK_PACKAGE_LESS_LEN(st,cmdlen){\
	if (sizeof(st) > cmdlen){\
	g_logger.debug("收到变长数据包 %s 接收长度 %d < 定义长度 %d ", #st,cmdlen,sizeof(st));\
	return false;}\
	}
/*--->[ 变长数据包, 检测接收的数据包长度是否等于需要的数据包长度 ]*/
#define _CHECK_VAR_PACKAGE_LEN(st, stlen, cmdlen){	\
	if(stlen != cmdlen){	\
	g_logger.debug("收到变长数据包 %s 接收长度 %d != 定义长度 %d", #st, cmdlen, stlen);		\
	return false;	\
	}	\
}




template<class _A, class _P1, class _P2> struct stunion2
{
	union
	{
		struct
		{
			_P1 _p1;
			_P2 _p2;
		};
		_A _value;
	};
};
template<class _A, class _P1, class _P2, class _P3> struct stunion3
{
	union
	{
		struct
		{
			_P1 _p1;
			_P2 _p2;
			_P3 _p3;
		};
		_A _value;
	};
};
template<class _A, class _P1, class _P2, class _P3, class _P4> struct stunion4
{
	union
	{
		struct
		{
			_P1 _p1;
			_P2 _p2;
			_P3 _p3;
			_P4 _p4;
		};
		_A _value;
	};
};
template<class _A, class _P1, class _P2, class _P3, class _P4, class _P5> struct stunion5
{
	union
	{
		struct
		{
			_P1 _p1;
			_P2 _p2;
			_P3 _p3;
			_P4 _p4;
			_P5 _p5;
		};
		_A _value;
	};
};
template<class _A, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6> struct stunion6
{
	union
	{
		struct
		{
			_P1 _p1;
			_P2 _p2;
			_P3 _p3;
			_P4 _p4;
			_P5 _p5;
			_P6 _p6;
		};
		_A _value;
	};
};
template<class _A, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7> struct stunion7
{
	union
	{
		struct
		{
			_P1 _p1;
			_P2 _p2;
			_P3 _p3;
			_P4 _p4;
			_P5 _p5;
			_P6 _p6;
			_P7 _p7;
		};
		_A _value;
	};
};
template<class _A, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8> struct stunion8
{
	union
	{
		struct
		{
			_P1 _p1;
			_P2 _p2;
			_P3 _p3;
			_P4 _p4;
			_P5 _p5;
			_P6 _p6;
			_P7 _p7;
			_P8 _p8;
		};
		_A _value;
	};
};

#define MINCHAR     0x80        
#define MAXCHAR     0x7f        
#define MINSHORT    0x8000      
#define MAXSHORT    0x7fff      
#define MINLONG     0x80000000  
#define MAXLONG     0x7fffffff  
#define MAXBYTE     0xff        
#define MAXWORD     0xffff      
#define MAXDWORD    0xffffffff  

#endif//_DEFINE_H__



