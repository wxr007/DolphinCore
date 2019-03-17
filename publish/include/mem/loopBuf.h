#ifndef _LOOPBUF_H__
#define _LOOPBUF_H__

#include <vector>
#include <queue>
#include<string.h>
#include<stdio.h>
#include <stdint.h>
#include "thread/lockObj.h"

//dy copyright
using namespace std;

#define DEF_LOOPBUF_SIZE1               1024*2+16-1
#define DEF_LOOPBUF_MAX_RESIZE        1024*1024*20      //最大用 缓存20m

//LoopBuf 模型 可读取的数据必然是连续的 数据流向---->
//|m_nPerpendLeft	    |m_nReadLeft	    |m_nWritLeft		    |
//|空闲数据块		    |可读取数据块	    |可写入数据块			|
//|m_ptrBegin		    |m_ptrRead		    |m_ptrWrite		        |

class  LoopBuf
{
	//从内部拷贝
public:
    LoopBuf(int nBufSize = DEF_LOOPBUF_SIZE1);
    ~LoopBuf();
    int    putData(const char* p,int nLen);             //压入数据
    int	   getReadDataLen();                            //获得可以读取的数据个数
    char*  getReadPtr();                                //获得读取数据的首地址
    int	   setReadPtr(int nLen);                        //设置 读取个数
private:
    int    putRightData(const char*p,int nLen);         //有足够的写入个数的时候才进行写入
    void   resizeNewAllData(int ninlen);				//重新分配内存
    void   RestructuringData();							//重组之前空闲的数据

private:
    int     m_nAllSize;     //数组的总大小
    int     m_nPerpendLeft; //数组前面空闲数据的个数
    int     m_nReadLeft;    //可以读的数据个数
    int     m_nWritLeft;    //可以写的数据个数
//ptr
    char*   m_ptrBegin;     //指向数组的首地址
    char*   m_ptrRead;      //指向 读取数据的首地址
    char*   m_ptrWrite;     //指向 写入数据的首地址
};


#define _Default_Len_			1024
template <class _Type>
class CLoopList : public CIntLock		//环形列表
{
public:
	typedef _Type*	iterator;
	typedef _Type*	const_iterator;
	typedef CLoopList<_Type> _Myt;
protected:
	uint32_t m_head;				//头部
    uint32_t m_tail;				//尾部
    uint32_t m_size;				//当前大小
    uint32_t m_capacity;			//最大容量
	_Type *pBuf;
public:
	CLoopList()
		 :m_head(0), m_tail(0), m_size(0)
	{
		pBuf = new _Type[_Default_Len_];//默认1024
		m_capacity = _Default_Len_;
	}
	CLoopList(unsigned int bufsize)
		: m_head(0), m_tail(0), m_size(0)
	{
		if( bufsize < 1)
		{
			pBuf = new _Type[_Default_Len_];
			m_capacity = _Default_Len_;
		}
		else
		{
			pBuf = new _Type[bufsize];
			m_capacity = bufsize;
		}
	}
	virtual ~CLoopList()
	{
		delete[] pBuf;
		pBuf = NULL;
		m_head = m_tail = m_size = m_capacity = 0;
	}
	_Type* pop_front()//弹出一个元素
	{
		if( IsEmpty() )
		{
			return NULL;
		}

		uint32_t old_head = m_head;
		m_head = (m_head + 1) % m_capacity;
		--m_size;
		return (_Type*)&pBuf[old_head];
	}

	bool push_back( _Type item)//压入一个元素
	{
		if ( IsFull() )
		{
			return false;
		}
		pBuf[m_tail] = item;
		m_tail = (m_tail + 1) % m_capacity;
		++m_size;
		return true;
	}

	unsigned int capacity() //返回容量
	{
		return m_capacity;
	}
	unsigned int size() //返回当前个数
	{
		return m_size;
	}
	
	bool IsFull() //是否满
	{
		return (m_size >= m_capacity);
	}
	bool IsEmpty()//是否空
	{
		return (m_size == 0);
	}
	void clear()
	{
		m_head = m_tail = m_size = 0;
	}
	bool merge(_Myt& LoopList)
	{
		if ( IsFull() )
		{
			return false;
		}
		if (LoopList.IsEmpty())
		{
			return false;
		}
		do
		{
			push_back(*(LoopList.pop_front()));
		}
		while (!IsFull() && (!LoopList.IsEmpty()));
		return true;
	}
};
#endif // _LOOPBUF_H__
