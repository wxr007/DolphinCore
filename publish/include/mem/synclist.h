#ifndef _SYNLIST_H__
#define _SYNLIST_H__

// *** ADDED BY HEADER FIXUP ***
#include <deque>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>
// *** END ***



//改文件 包含了 stl 线程安全的 vector map list 对象


#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <stack>
#include "define/define.h"      //_STD_BEGIN
#include "thread/lockObj.h"    //CIntLock
using namespace std;

#include <iostream>
#include <algorithm>




#define DEF1_FUNCTOR_BEGIN(tname,_arg,_result)			struct tname{	\
														struct _IF:public std::unary_function< _arg, _result >{	\
														_result operator()( _arg _p1 )


#define DEF2_FUNCTOR_BEGIN(tname,_arg1,_arg2,_result)	struct tname{	\
														struct _IF:public binary_function< _arg1, _arg2 , _result >{	\
														_result operator()( _arg1 _p1 , _arg2 _p2 )



#define DEF_FUNCTOR_END							;};};


template < class _Ty, bool _Tr = true > struct nil_f
{
	__inline bool operator()(_Ty& _P0)
	{
		return _Tr;
	}
};


_STD_BEGIN

template < class _Kty,
         class _Pr = less<_Kty>,
         class _Alloc = allocator<_Kty> >
class CSyncSet : public set< _Kty, _Pr, _Alloc >, public CIntLock
{
public:
	typedef set< _Kty, _Pr, _Alloc > tbase;
};

template < class _Kty,
         class _Ty,
         class _Pr = less<_Kty>,
         class _Alloc = allocator<pair<const _Kty, _Ty> > >
class CSyncMap : public map< _Kty, _Ty, _Pr, _Alloc >, public CIntLock
{
public:
	typedef map< _Kty, _Ty, _Pr, _Alloc > tbase;
};

template < class _Kty,
         class _Ty,
         class _Pr = less<_Kty>,
         class _Alloc = allocator<pair<const _Kty, _Ty> > >
class CSyncMultimap : public multimap< _Kty, _Ty, _Pr, _Alloc >, public CIntLock
{
public:
	typedef multimap< _Kty, _Ty, _Pr, _Alloc > tbase;
};


template < class _Ty, class _Ax = allocator<_Ty> >
class CSyncVector : public vector< _Ty, _Ax >, public CIntLock
{
public:
	typedef vector< _Ty, _Ax > tbase;
};




template < class _Ty,	class _Ax = allocator<_Ty> >
class CSyncList : public list< _Ty, _Ax >, public CIntLock
{
public:
	typedef list< _Ty, _Ax > tbase;
};

template < class _Ty,	class _Ax = allocator<_Ty> >
class CSyncQueue : public queue< _Ty, deque<_Ty, _Ax> >, public CIntLock
{
public:
	typedef queue< _Ty, _Ax > tbase;

	virtual void Push(_Ty p)//该函数并未使用
	{
		AILOCKT(*this);
		this->push(p);
	};



	virtual  bool Pop(_Ty& p)//该函数并未使用
	{
		AILOCKT(*this);

		if(this->empty())
			return false;

		p = this->front();
		this->pop();
		return true;
	};

//	deque<_Ty, _Ax>& get_s()//该函数并未使用
//	{
//	    //TODO:winodws下 此处代码根本没有 用处
//		//deque<_Ty, _Ax>* pc =NULL;// (deque<_Ty, _Ax>*)&_Get_s();
//		return *pc;
//	}
};

template < class _Ty,	class _Ax = allocator<_Ty> >
class CSyncStack : public deque< _Ty, _Ax >, public CIntLock
{
public:

	typedef deque< _Ty, _Ax > tbase;

	virtual void Push(_Ty p)//该函数并未使用
	{
		AILOCKT(*this);
		this->push_back(p);
	};

	virtual bool Pop(_Ty& p)//该函数并未使用
	{
		AILOCKT(*this);

		if(this->empty())
			return false;

		p = this->back();
		this->pop_back();
		return true;
	};
};


template < class _InputIter , class _Function >
void for_each_ret(_InputIter __first, _InputIter __last, _Function __f)
{
	for(; __first != __last; ++__first)
	{
		if(!__f(*__first))
		{
			return;
		}
	}

	return;
}

template < class _Ty >
int push2vector(std::vector< _Ty > &v, void* pbuf, int ncount)
{
	if(ncount > 0)
	{
		size_t nold = v.size();
		v.resize(nold + ncount);
		CopyMemory(&v[nold], pbuf, ncount * sizeof(_Ty));
		return ncount;
	}

	return 0;
}

template< class TList >
inline void clearplist(TList& list)
{
	for(typename TList::iterator it = list.begin(); it != list.end(); ++it)
	{
		typename TList::value_type pinfo = (*it);
		DEFINE_DEL_ONE(pinfo);
		SAFE_DELETE(pinfo);
	}

	list.clear();
}

template< class TList >
inline void clearsyncplist(TList& list)
{
	AILOCKT(list);

	for(typename TList::iterator it = list.begin(); it != list.end(); ++it)
	{
		typename TList::value_type pinfo = (*it);
		DEFINE_DEL_ONE(pinfo);
		SAFE_DELETE(pinfo);
	}

	list.clear();
}


template< class TMap >
inline void clearpmap2(TMap& vmap)
{
	for(typename TMap::iterator it = vmap.begin(); it != vmap.end(); ++it)
	{
	    DEFINE_DEL_ONE(it->second);
		SAFE_DELETE(it->second);
	}

	vmap.clear();
}

template< class TMap >
inline void clearsyncpmap2(TMap& vmap)
{
	AILOCKT(vmap);

	for(typename TMap::iterator it = vmap.begin(); it != vmap.end(); ++it)
	{
        DEFINE_DEL_ONE(it->second);
		SAFE_DELETE(it->second);
	}

	vmap.clear();
}

template< class TMap, class ValueT >
inline typename TMap::iterator mapfindby2(TMap& vmap, const ValueT &value)
{
	for(typename TMap::iterator it = vmap.begin(); it != vmap.end(); ++it)
	{
		if(it->second == value)
		{
			return it;
		}
	}

	return vmap.end();
}

template< class TMap, class ValueT >
inline bool mapdelby2(TMap& vmap, const ValueT &value)
{
	typename TMap::iterator it = mapfindby2(vmap, value);

	if(it != vmap.end())
	{
		vmap.erase(it);
		return true;
	}

	return false;
}

template< class TMap, class KeyT, class ValueT >
inline typename TMap::iterator mmapfind(TMap& vmap, const KeyT &key, const ValueT &value)
{
	std::pair<typename TMap::iterator, typename TMap::iterator> its = vmap.equal_range(key);

	for(typename TMap::iterator it = its.first; it != its.second; it++)
	{
		if(it->second == value)
		{
			return it;
		}
	}

	return vmap.end();
}

template< class TMap, class KeyT, class ValueT >
inline bool mmapdelete(TMap& vmap, const KeyT &key, const ValueT &value)
{
	typename TMap::iterator it = mmapfind(vmap, key, value);

	if(it != vmap.end())
	{
		vmap.erase(it);
		return true;
	}

	return false;
}

template < class tVec>
inline void SimpleRemoveFromArray(tVec & vec, int index)
{
	vec[index] = vec[vec.size()-1];
	vec.resize(vec.size() - 1);
}

_STD_END



#endif // _SYNLIST_H__