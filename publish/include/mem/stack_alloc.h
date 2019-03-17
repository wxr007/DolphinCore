/*------------- stack_alloc.h
*
* Copyright (C): www.7cool.cn (2010)
* Author       : 
* Version      : V1.01
* Date         : 2010/11/26 13:09:54
*
*/ 
/*************************************************************
*
*************************************************************/
#ifndef _STACK_ALLOC_H__
#define _STACK_ALLOC_H__

#ifdef STLPORT
#include "mem/stlport_stack_alloc.h"
#else
#include "frameAllocator.h"
#include <xmemory>
#include <string>

/*************************************************************/
_STD_BEGIN
template<class _Ty> inline
_Ty _FARQ *_StackAllocate(_SIZT _Count, _Ty _FARQ *)
{
	return ((_Ty _FARQ *)tlsFrameAllocator.alloc((uint32_t)_Count * sizeof(_Ty)));
}

template<class _Ty>

class stack_allocator
	: public _Allocator_base<_Ty>
{
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;


	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
	struct rebind
	{
		typedef stack_allocator<_Other> other;
	};

	pointer address(reference _Val) const
	{
		return (&_Val);
	}

	const_pointer address(const_reference _Val) const
	{
		return (&_Val);
	}

	stack_allocator()
	{
	}

	stack_allocator(const stack_allocator<_Ty>&)
	{
	}

	template<class _Other>
	stack_allocator(const stack_allocator<_Other>&)
	{
	}

	template<class _Other>
	stack_allocator<_Ty>& operator=(const stack_allocator<_Other>&)
	{
		return (*this);
	}

	void deallocate(pointer _Ptr, size_type)
	{
	}

	pointer allocate(size_type _Count)
	{
		return (_StackAllocate(_Count, (pointer)0));
	}

	pointer allocate(size_type _Count, const void _FARQ *)
	{
		return (allocate(_Count));
	}

	void construct(pointer _Ptr, const _Ty& _Val)
	{
		_Construct(_Ptr, _Val);
	}

	void destroy(pointer _Ptr)
	{
		_Destroy(_Ptr);
	}

	_SIZT max_size() const
	{
		_SIZT _Count = (_SIZT)(-1) / sizeof(_Ty);
		return (0 < _Count ? _Count : 1);
	}
};

typedef basic_string<char, char_traits<char>, stack_allocator<char> >	stack_string;
typedef basic_string<wchar_t, char_traits<wchar_t>,	stack_allocator<wchar_t> > stack_wstring;

_STD_END
#endif

#endif //_STACK_ALLOC_H__
