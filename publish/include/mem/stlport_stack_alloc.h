/*------------- stlport_stack_alloc.h
*
* Copyright (C): www.7cool.cn (2010)
* Author       : 
* Version      : V1.01
* Date         : 2010/11/26 13:10:07
*
*/ 
/*************************************************************
*
*************************************************************/
#ifndef _STLPORT_STACK_ALLOC_H__
#define _STLPORT_STACK_ALLOC_H__

#ifdef STLPORT
#include <algorithm>
#include <string>
#include "frameAllocator.h"
/*************************************************************/

_STD_BEGIN


template <class _Tp >
struct stack_allocator
{
	typedef _Tp        value_type;
	typedef value_type *       pointer;
	typedef const _Tp* const_pointer;
	typedef _Tp&       reference;
	typedef const _Tp& const_reference;
	typedef size_t     size_type;
	typedef ptrdiff_t  difference_type;

	mallocState* getState()
	{
		return m_state;
	};
	void setState(mallocState* state)
	{
		m_state = state;
	};

	template <class _OtherTp>
	stack_allocator(stack_allocator<_OtherTp > const& other)
	{
		m_state = other.getState();
	};

	stack_allocator(const mallocState* state = NULL)
	{
		if(NULL != state)
		{
			m_state = (mallocState*)state;
		}
		else
		{
			m_state = tlsFrameAllocator.getmallocState();
		}
	};

	template <class _Other>
	struct rebind
	{
		typedef stack_allocator<_Other> other;
	};

	pointer allocate(size_type n, void* p = NULL)
	{
		if(n == 0)
			return NULL;

		char *ret = (char *)m_state->alloc(n * sizeof(value_type));

		if(ret)
		{
			return reinterpret_cast<_Tp*>(ret);
		}
		else
		{
			return NULL;
		}
	};


	void deallocate(pointer p, size_type n)
	{
		if(p == 0)
			return;

		m_state->_free(p);
	};

	pointer address(reference __x) const
	{
		return &__x;
	}
	const_pointer address(const_reference __x) const
	{
		return &__x;
	}
	size_type max_size() const
	{
		return m_state->m_end - m_state->m_cur;
	}
	void construct(pointer __p, const_reference __val)
	{
		new(__p) _Tp(__val);
	}
	void destroy(pointer __p)
	{
		__p->~_Tp();
	}

	void reset()
	{
		m_state->reset();
	};


	bool operator == (stack_allocator const& other) const
	{
		return m_state->m_beg == other.m_state->m_beg;
	};

	bool operator != (stack_allocator const& other) const
	{
		return !(*this == other);
	};
protected:
	mallocState* m_state;
};


typedef basic_string< char, char_traits<char>, stack_allocator<char> >	stack_string;
typedef basic_string<wchar_t, char_traits<wchar_t>,	stack_allocator<wchar_t> > stack_wstring;



typedef basic_istringstream<char, char_traits<char>, stack_allocator<char > > stack_istringstream;
typedef basic_ostringstream<char, char_traits<char>, stack_allocator<char > > stack_ostringstream;
typedef basic_stringstream<char, char_traits<char>, stack_allocator<char > > stack_stringstream;


_STD_END

#endif

#endif //_STLPORT_STACK_ALLOC_H__
