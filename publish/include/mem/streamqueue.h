#ifndef _STREAMQUEUE_H__
#define _STREAMQUEUE_H__


#include <deque>
#include <queue>
#include "mem/lookaside_alloc.h"//内存池
#include "thread/lockObj.h"       //同步对象
#include "mem/lookaside_alloc.h"

/*************************************************************/
class CSafeMsgQueue : public CIntLock
{
public:
	void clear()
	{
		AILOCKT(*this);

		while(!m_queue.empty())
		{
			m_queue.pop();
		}
	}

	size_t size()
	{
		AILOCKT(*this);
		return m_queue.size();
	}

	bool empty()
	{
		AILOCKT(*this);
		return m_queue.empty();
	}

    //TODO : unused
	template < class _RET >
	static __inline bool NewPushBuffer(_RET*& msg, size_t size)
	{
		size = ROUNDNUM2(size + 8 + 1, 8);
		msg = (_RET*)__mt_char_alloc.allocate(size);
		return (msg != NULL);
	};

	template < class _RET >
	bool Push(_RET* msg)
	{
		if(msg)
		{
			AILOCKT(*this);
			m_queue.push((char*)msg);
			return true;
		}

		return false;
	};

	template < class _RET >
	bool Pop(_RET*& msg)
	{
		AILOCKT(*this);

		if(!m_queue.empty())
		{
			msg = (_RET*)m_queue.front();
			m_queue.pop();
			return (msg != NULL);
		}

		msg = NULL;
		return false;
	};

	template < class _RET >
	_RET* Pop()
	{
		_RET* msg;

		if(Pop(msg))
		{
			return msg;
		};

		return NULL;
	};

	template < class _RET >
	static __inline bool FreePushBuffer(_RET* msg)
	{
		__mt_char_alloc.deallocate(msg);
		return true;
	};
private:
	std::queue<char*> m_queue;
};



/*/具体使用

先弹出来 使用 使用完了以后才是删除对象
	while(m_msg.Pop(pbufmsg))
		FreePacketBuffer(pbufmsg);

	m_msg.clear();

//
	while(m_msg.Pop(pbufmsg))//先弹出来
	{
		bufferparam.bofreebuffer = true;
		msgParse(&pbufmsg->cmdBuffer, pbufmsg->cmdsize, &bufferparam);//这里是使用

		if(bufferparam.bofreebuffer)
		{
			FreePacketBuffer(pbufmsg);//然后销毁

*/


#endif // _STREAMQUEUE_H__
