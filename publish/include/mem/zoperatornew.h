/*------------- zoperatornew.h
*
* Copyright (C): www.7cool.cn (2010)
* Author       :
* Version      : V1.01
* Date         : 2010/11/23 19:37:30
*
*/
/*************************************************************
*
*************************************************************/
#ifndef _ZOPERATOR_NEW_H__
#define _ZOPERATOR_NEW_H__


#include "platform/platform.h"


/*************************************************************/

#define _poolallocator_(T)		safe_lookaside_allocator< T >
//////////////////////////////////////////////////////////////
#define  DEC_OP_NEW(T)\
private:\
	static _poolallocator_(T)	 m_##T##allocator;\
public:\
	static int32_t	m_nrefcount;\
/*
**new
*/\
static void* operator  new(size_t n)\
{\
	if(n == sizeof(T))\
	{\
		InterlockedIncrement_Q(&m_nrefcount);\
		return LOOKASIDE_GETMEM(m_##T##allocator);\
	}\
	else\
	{\
		return ::operator new(n);\
	}; \
}\
/*
**new
*/\
template<class __P1>\
static void* operator  new(size_t n, __P1 p1)\
{\
	if(n == sizeof(T))\
	{\
		InterlockedIncrement_Q(&m_nrefcount); \
		return LOOKASIDE_GETMEM(m_##T##allocator);\
	}\
	else\
	{\
		return ::operator new(n);\
	}; \
}\
/*
**new
*/\
template<class __P1, class __P2>\
static void* operator  new(size_t n, __P1 p1, __P2 p2)\
{\
	if(n == sizeof(T))\
	{\
		InterlockedIncrement_Q(&m_nrefcount); \
		return LOOKASIDE_GETMEM(m_##T##allocator);\
	}\
	else\
	{\
		return ::operator new(n);\
	}; \
}\
/*
**new
*/\
template<class __P1, class __P2, class __P3>\
static void* operator  new(size_t n, __P1 p1, __P2 p2, __P3 p3)\
{\
	if(n == sizeof(T))\
	{\
		InterlockedIncrement_Q(&m_nrefcount); \
		return LOOKASIDE_GETMEM(m_##T##allocator);\
	}\
	else\
	{\
		return ::operator new(n);\
	}; \
}\
/*
**new
*/\
template<class __P1, class __P2, class __P3, class __P4>\
static void* operator  new(size_t n, __P1 p1, __P2 p2, __P3 p3, __P4 p4)\
{\
	if(n == sizeof(T))\
	{\
		InterlockedIncrement_Q(&m_nrefcount); \
		return LOOKASIDE_GETMEM(m_##T##allocator);\
	}\
	else\
	{\
		return ::operator new(n);\
	}; \
}\
/*
**delete
*/\
static void operator  delete(void *p, size_t n)\
{\
	if(p)\
	{\
		if(n == sizeof(T))\
		{\
			InterlockedDecrement_Q(&m_nrefcount); \
			m_##T##allocator.freemem((T*)p);\
		}\
		else\
		{\
			return ::operator delete(p);\
		};\
	};\
}

//////////////////////////////////////////////////////////////
#define  IMP_OP_NEW(T)	_poolallocator_(T) T::m_##T##allocator;		int32_t T::m_nrefcount=0;


#endif //_ZOPERATOR_NEW_H__