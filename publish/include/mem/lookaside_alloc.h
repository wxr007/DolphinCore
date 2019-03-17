#ifndef _LOOKASIDE_ALLOC_H__
#define _LOOKASIDE_ALLOC_H__

//内存池的实现
#include "define/define.h"
#include "thread/lockObj.h"

#include <algorithm>
#include <assert.h>
#include <memory>



#define _FARQ
#define _SIZT size_t
#define _PDFT ptrdiff_t

/*************************************************************/
using namespace std;

template<class T>
inline	void constructInPlace(T  *_Ptr)
{
	new((void  *)_Ptr) T(T());//在该地址上调用构造函数 也就是说 手动调用构造函数
}

template<class _Ty, class _TParam>
inline	void constructInPlace(_Ty  *_Ptr, _TParam param)
{
	new((void  *)_Ptr) _Ty(param);
}

template<class _Ty, class _TParam1, class _TParam2>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2)
{
	new((void  *)_Ptr) _Ty(param1, param2);
}

template<class _Ty, class _TParam1, class _TParam2, class _TParam3>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2, _TParam3 param3)
{
	new((void  *)_Ptr) _Ty(param1, param2 , param3);
}

template<class _Ty, class _TParam1, class _TParam2, class _TParam3, class _TParam4>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4)
{
	new((void  *)_Ptr) _Ty(param1, param2 , param3, param4);
}

template<class _Ty, class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5)
{
	new((void  *)_Ptr) _Ty(param1, param2 , param3, param4, param5);
}
template<class _Ty, class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5, class _TParam6>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5, _TParam6 param6)
{
	new((void  *)_Ptr) _Ty(param1, param2, param3, param4, param5, param6);
}
template<class _Ty, class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5, class _TParam6, class _TParam7>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5, _TParam6 param6, _TParam7 param7)
{
	new((void  *)_Ptr) _Ty(param1, param2, param3, param4, param5, param6, param7);
}
template<class _Ty, class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5, class _TParam6, class _TParam7, class _TParam8>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5, _TParam6 param6, _TParam7 param7, _TParam8 param8)
{
	new((void  *)_Ptr) _Ty(param1, param2, param3, param4, param5, param6, param7, param8);
}
template<class _Ty, class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5, class _TParam6, class _TParam7, class _TParam8, class _TParam9>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5, _TParam6 param6, _TParam7 param7, _TParam8 param8, _TParam9 param9)
{
	new((void  *)_Ptr) _Ty(param1, param2, param3, param4, param5, param6, param7, param8, param9);
}
template<class _Ty, class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5, class _TParam6, class _TParam7, class _TParam8, class _TParam9, class _TParam10>
inline	void constructInPlace(_Ty  *_Ptr, _TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5, _TParam6 param6, _TParam7 param7, _TParam8 param8, _TParam9 param9, _TParam10 param10)
{
	new((void  *)_Ptr) _Ty(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
}
template <class T>
inline void destructInPlace(T* p)
{
	p->~T();
}

template<class _T>
inline	void constructInPlaceCount(_T  *_Ptr, size_t count)
{
	for(size_t i = 0; i < count; ++i)
	{
		constructInPlace(_Ptr++);
	}
}


template <class T>
inline void destructInPlaceCount(T* p, size_t count)
{
	for(size_t i = 0; i < count; ++i)
	{
		destructInPlace(p++);
	}
}
extern size_t gLookasideAllocSize;

_STD_BEGIN

template<class _Ty>
struct lookaside_node //list
{
	typedef lookaside_node< _Ty > _Myt;
	unsigned char data[sizeof(_Ty)];
	_Myt* next;
};

template < class _Ty, size_t blockNodeNum = 64 >
class lookaside_allocator
{
protected:
	typedef  lookaside_node< _Ty > _MyNode;

	_MyNode * m_freePool;

	struct stPool //在 lookaside_node 基础上又包了一层 list
	{
		_MyNode nodes[blockNodeNum];
		stPool* next;
	};
	stPool*		mPools;//头节点
public:

#ifdef STLPORT
	typedef _Ty        value_type;
	typedef value_type *       pointer;
	typedef const _Ty* const_pointer;
	typedef _Ty&       reference;
	typedef const _Ty& const_reference;
	typedef size_t     size_type;
	typedef ptrdiff_t  difference_type;
#else
	typedef _Ty _FARQ *pointer;
	typedef _Ty _FARQ& reference;
	typedef const _Ty _FARQ *const_pointer;
	typedef const _Ty _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;
#endif

	lookaside_allocator() : m_freePool(NULL) , mPools(NULL)
	{
	}

	virtual ~lookaside_allocator()
	{
		freeall();
	}

	void freeall()
	{
		while(mPools)
		{
			stPool* p = mPools;
			mPools = p->next;
			free(p);
			gLookasideAllocSize -= sizeof(stPool);
		}

		m_freePool = NULL;
	}

	lookaside_allocator(const lookaside_allocator<_Ty>&) //m_list(NULL)
	{
		assert(0);
	}

	void deallocate(pointer _Ptr, size_type size)
	{
		assert(size == 1);
		_MyNode* pNode = (_MyNode*)_Ptr;
		pNode->next = m_freePool;
		m_freePool = pNode;
	}

	void destroy(pointer _Ptr)
	{
		_Ptr->~_Ty();
	}
#ifdef _DEBUG
	void addPoolBlock(const char* fileName, int line)
	{
		stPool* pool = (stPool*)_malloc_dbg(sizeof(stPool), _NORMAL_BLOCK, fileName, line);

		if(pool)
		{
			gLookasideAllocSize += sizeof(stPool);
			pool->next = mPools;
			mPools = pool;
			_MyNode* p = &pool->nodes[0];

			for(size_t i = 0 ; i < blockNodeNum - 1; ++i)
			{
				p[i].next = &p[i+1];
			}

			p[blockNodeNum-1].next = NULL;
			m_freePool = p;
		}
	}

	pointer allocate(size_type _Count, const char* fileName, int line)
	{
		assert(_Count == 1);

		if(!m_freePool)
			addPoolBlock(fileName, line);

		_MyNode* pNode = m_freePool;
		m_freePool = m_freePool->next;
		pNode->next = NULL;
		return (pointer)&pNode->data;
	}

#else

	void addPoolBlock()
	{
		stPool* pool = (stPool*)malloc(sizeof(stPool));
		gLookasideAllocSize += sizeof(stPool);
		pool->next = mPools;
		mPools = pool;
		_MyNode* p = &pool->nodes[0];

		for(size_t i = 0 ; i < blockNodeNum - 1; ++i)
		{
			p[i].next = &p[i+1];
		}

		p[blockNodeNum-1].next = NULL;
		m_freePool = p;
	}

	pointer allocate(size_type _Count)
	{
		if(!m_freePool)
			addPoolBlock();

		_MyNode* pNode = m_freePool;
		m_freePool = m_freePool->next;
		pNode->next = NULL;
		return (pointer)&pNode->data;
	}
#endif

#ifdef _DEBUG
	void* getmem(const char* fileName, int line)
	{
		return allocate(1, fileName, line);
	}
	pointer alloc(const char* fileName, int line)
	{
		pointer _Ptr = allocate(1, fileName, line);
		constructInPlace(_Ptr);
		return _Ptr;
	}

	template < class _TParam>
	pointer alloc(_TParam param, const char* fileName, int line)
	{
		pointer _Ptr = allocate(1, fileName, line);
		constructInPlace(_Ptr, param);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2>
	pointer alloc(_TParam1 param1, _TParam2 param2, const char* fileName, int line)
	{
		pointer _Ptr = allocate(1, fileName, line);
		constructInPlace(_Ptr, param1, param2);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3>
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, const char* fileName, int line)
	{
		pointer _Ptr = allocate(1, fileName, line);
		constructInPlace(_Ptr, param1, param2, param3);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3, class _TParam4>
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, const char* fileName, int line)
	{
		pointer _Ptr = allocate(1, fileName, line);
		constructInPlace(_Ptr, param1, param2, param3, param4);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5 >
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5, const char* fileName, int line)
	{
		pointer _Ptr = allocate(1, fileName, line);
		constructInPlace(_Ptr, param1, param2, param3, param4, param5);
		return _Ptr;
	}

#else
	void* getmem()
	{
		return allocate(1);
	}
	pointer alloc()
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr);
		return _Ptr;
	}

	template < class _TParam>
	pointer alloc(_TParam param)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr, param);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2>
	pointer alloc(_TParam1 param1, _TParam2 param2)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr, param1, param2);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3>
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr, param1, param2, param3);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3, class _TParam4>
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr, param1, param2, param3, param4);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5 >
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr, param1, param2, param3, param4, param5);
		return _Ptr;
	}
#endif
	void freemem(void* _Ptr)
	{
		return deallocate(((pointer)_Ptr), 1);
	}

	void freeobj(void* _Ptr)
	{
		destroy(((pointer)_Ptr));
		deallocate(((pointer)_Ptr), 1);
	}
};


//safe_lookaside_allocator 线程安全 内存池
template < class _Ty, size_t blockNodeNum = 64 >
class safe_lookaside_allocator : public lookaside_allocator< _Ty, blockNodeNum >, protected CIntLock
{
public:

    typedef typename lookaside_allocator< _Ty, blockNodeNum >::pointer pointer;
    typedef typename lookaside_allocator< _Ty, blockNodeNum >::stPool stPool;
    using lookaside_allocator< _Ty, blockNodeNum >::mPools;
    using lookaside_allocator< _Ty, blockNodeNum >::m_freePool;
    using lookaside_allocator< _Ty, blockNodeNum >::allocate;

	virtual ~safe_lookaside_allocator()
	{
		freeall();
	}

	void freeall()
	{
		AILOCKT(*this);

		while(mPools)
		{
			stPool* p = mPools;
			mPools = p->next;
			free(p);
			gLookasideAllocSize -= sizeof(stPool);
		}

		m_freePool = NULL;
	}

#ifdef _DEBUG
	void* getmem(const char* fileName, int line)
	{
		AILOCKT(*this);
		return allocate(1, fileName, line);
	}
	pointer alloc(const char* fileName, int line)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1, fileName, line);
		}
		while(false);

		constructInPlace(_Ptr);
		return _Ptr;
	}

	template < class _TParam>
	pointer alloc(_TParam param, const char* fileName, int line)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1, fileName, line);
		}
		while(false);

		constructInPlace(_Ptr, param);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2>
	pointer alloc(_TParam1 param1, _TParam2 param2, const char* fileName, int line)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1, fileName, line);
		}
		while(false);

		constructInPlace(_Ptr, param1, param2);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3>
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, const char* fileName, int line)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1, fileName, line);
		}
		while(false);

		constructInPlace(_Ptr, param1, param2, param3);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3, class _TParam4>
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, const char* fileName, int line)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1, fileName, line);
		}
		while(false);

		constructInPlace(_Ptr, param1, param2, param3, param4);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5 >
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5, const char* fileName, int line)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1, fileName, line);
		}
		while(false);

		constructInPlace(_Ptr, param1, param2, param3, param4, param5);
		return _Ptr;
	}

#else
	void* getmem()
	{
		AILOCKT(*this);
		return allocate(1);
	}
	pointer alloc()
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1);
		}
		while(false);

		constructInPlace(_Ptr);
		return _Ptr;
	}

	template < class _TParam>
	pointer alloc(_TParam param)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1);
		}
		while(false);

		constructInPlace(_Ptr, param);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2>
	pointer alloc(_TParam1 param1, _TParam2 param2)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1);
		}
		while(false);

		constructInPlace(_Ptr, param1, param2);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3>
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1);
		}
		while(false);

		constructInPlace(_Ptr, param1, param2, param3);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3, class _TParam4>
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1);
		}
		while(false);

		constructInPlace(_Ptr, param1, param2, param3, param4);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2, class _TParam3, class _TParam4, class _TParam5 >
	pointer alloc(_TParam1 param1, _TParam2 param2, _TParam3 param3, _TParam4 param4, _TParam5 param5)
	{
		pointer _Ptr;

		do
		{
			AILOCKT(*this);
			_Ptr = allocate(1);
		}
		while(false);

		constructInPlace(_Ptr, param1, param2, param3, param4, param5);
		return _Ptr;
	}
#endif

	void freemem(void* _Ptr)
	{
		AILOCKT(*this);
		return this->deallocate(((pointer)_Ptr), 1);
	}

	void freeobj(void* _Ptr)
	{
		destroy(((pointer)_Ptr));

		do
		{
			AILOCKT(*this);
			deallocate(((pointer)_Ptr), 1);
		}
		while(false);
	}
};

class CSimpleAllocator
{
public:
	char* getmem(int n);
	void freemem(void* p);
	char* allocate(int n);
	void deallocate(void* p);

	template < class _TP >
	bool allocate(_TP*& pret, int n)
	{
		pret = (_TP*)allocate(sizeof(_TP) * n);
		return (pret != NULL);
	}

	static CSimpleAllocator _ty_alloc;
protected:
    //静态变量必须初始化才能使用
	static std::allocator< char > _ty_alloc_0_128;
	static safe_lookaside_allocator< char[256], 64 > _ty_alloc_128;
	static safe_lookaside_allocator< char[512], 48 > _ty_alloc_256;
	static safe_lookaside_allocator< char[512*2], 32 > _ty_alloc_512;
	static safe_lookaside_allocator< char[512*3], 16 > _ty_alloc_512x2;
	static safe_lookaside_allocator< char[512*4], 16 > _ty_alloc_512x3;
	static safe_lookaside_allocator< char[512*5], 16 > _ty_alloc_512x4;
};

_STD_END

#define __mt_char_alloc  CSimpleAllocator::_ty_alloc

#ifdef _DEBUG
#define LOOKASIDE_GETMEM(lookasideAllocator)  (lookasideAllocator).getmem(__FILE_FUNC__,__LINE__)
#define LOOKASIDE_ALLOC(lookasideAllocator) (lookasideAllocator).alloc(__FILE_FUNC__,__LINE__)
#define LOOKASIDE_PALLOC(lookasideAllocator,param) (lookasideAllocator).alloc(param,__FILE_FUNC__,__LINE__)
#define LOOKASIDE_PALLOC1(lookasideAllocator,param) (lookasideAllocator).alloc(param,__FILE_FUNC__,__LINE__)
#define LOOKASIDE_PALLOC2(lookasideAllocator,param1,param2) (lookasideAllocator).alloc(param1,param2,__FILE_FUNC__,__LINE__)
#define LOOKASIDE_PALLOC3(lookasideAllocator,param1,param2,param3) (lookasideAllocator).alloc(param1,param2,param3,__FILE_FUNC__,__LINE__)
#define LOOKASIDE_PALLOC4(lookasideAllocator,param1,param2,param3,param4) (lookasideAllocator).alloc(param1,param2,param3,param4,__FILE_FUNC__,__LINE__)
#define LOOKASIDE_PALLOC5(lookasideAllocator,param1,param2,param3,param4,param5) (lookasideAllocator).alloc(param1,param2,param3,param4,param5,__FILE_FUNC__,__LINE__)
#else
#define LOOKASIDE_GETMEM(lookasideAllocator)  (lookasideAllocator).getmem()
#define LOOKASIDE_ALLOC(lookasideAllocator) (lookasideAllocator).alloc()
#define LOOKASIDE_PALLOC(lookasideAllocator,param) (lookasideAllocator).alloc(param)
#define LOOKASIDE_PALLOC1(lookasideAllocator,param) (lookasideAllocator).alloc(param)
#define LOOKASIDE_PALLOC2(lookasideAllocator,param1,param2) (lookasideAllocator).alloc(param1,param2)
#define LOOKASIDE_PALLOC3(lookasideAllocator,param1,param2,param3) (lookasideAllocator).alloc(param1,param2,param3)
#define LOOKASIDE_PALLOC4(lookasideAllocator,param1,param2,param3,param4) (lookasideAllocator).alloc(param1,param2,param3,param4)
#define LOOKASIDE_PALLOC5(lookasideAllocator,param1,param2,param3,param4,param5) (lookasideAllocator).alloc(param1,param2,param3,param4,param5)
#endif


#endif // _LOOKASIDE_ALLOC_H__
