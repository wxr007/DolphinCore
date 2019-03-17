#ifndef _FRAMEALLOCATOR_H__
#define _FRAMEALLOCATOR_H__

#include "thread/lockObj.h"

#include <assert.h>

// #include "znoncopyable.h"
#include <algorithm>
#include "logging/logging/zLogger.h"
// #include "ThreadFactor/Cthread.h"
#include "mem/lookaside_alloc.h"

using namespace std;


struct mallocState: private zNoncopyable
{
public:
	char *m_beg;
	char *m_end;
	char *m_cur;
	char *m_last;
	int m_nMaxSize;
	int m_nMaxFrameAllocation;
public:
	void Init(char *beg, char *end);
	void Init(char *beg, int nSize);
	void Init(mallocState* other);
	void Uninit();
	mallocState();
	mallocState(char *beg, char *end);
	mallocState(char *beg, int nSize);
	mallocState(const mallocState& other);
	void reset();
	void* alloc(uint64_t allocSize);
	void _free(void* p);
	void setCurSize(const int waterMark);
	int getCurSize();
	int getBufferSize();
	int getUseMaxSize();
	char* getbeg();
	bool issysmalloc(void* p);
};

struct stStackFrameAllocator
{
	static uint64_t m_initframeSize;
	int64_t m_currthread;
	mallocState* m_State;
	char Statebuf[sizeof(mallocState)];
public:

	void init(uint64_t frameSize, bool bocheckthreadobj = false);

	void destroy()
	{
		uninit();
	}
	//~stStackFrameAllocator(){uninit();}

	void uninit()
	{
		if(m_State != NULL
		        &&  m_State->m_beg != NULL
		        && m_State->m_nMaxSize != 0)
		{
			char* p = m_State->getbeg();
			m_State->Uninit();
			destructInPlace(m_State);
			m_State = NULL;
			free(p);
		}
	}
	void* alloc(uint64_t allocSize)
	{
		return m_State->alloc(allocSize);
	}
	void* allocNoAlign(uint64_t allocSize)
	{
		return m_State->alloc(allocSize);
	}
	void _free(void* p)
	{
		m_State->_free(p);
	}
	void setWaterMark(const uint64_t waterMark)
	{
		m_State->setCurSize((int)waterMark);
	}
	uint64_t getWaterMark()
	{
		return (uint64_t)m_State->getCurSize();
	}
	uint64_t getHighWaterMark()
	{
		return (uint64_t)m_State->getBufferSize();
	}
	mallocState* getmallocState()
	{
		return m_State;
	}
};


extern _TH_VAR(stStackFrameAllocator, tls_FrameAllocator);
#define tlsFrameAllocator	_TH_VAR_GET(tls_FrameAllocator)

class SetFrameAllocator
{
private:
	uint64_t m_dwWaterMark;
	mallocState* m_State;
	std::vector<uint8_t*> m_vps;
public:
	SetFrameAllocator(mallocState* pState = NULL)
	{
		if(pState)
		{
			m_State = pState;
		}
		else
		{
			m_State = tlsFrameAllocator.getmallocState();
		}

		m_dwWaterMark = m_State->getCurSize();
	}
	~SetFrameAllocator()
	{
		if(m_State)
		{
			m_State->setCurSize(INT32(m_dwWaterMark));
		}

		if(m_vps.size() > 0)
		{
			for(int i = 0; i < (int)m_vps.size(); i++)
			{
				uint8_t* p = m_vps[i];

				if(p)
				{
					m_State->_free(p);
				}
			}

			m_vps.clear();
		}
	}
	void* alloc(size_t size)
	{
		if(m_State)
		{
			void* p = m_State->alloc(size);

			if(m_State->issysmalloc(p))
			{
				m_vps.push_back((unsigned char *)p);
			}

			return p;
		}
		else
		{
			return NULL;
		}
	}
};


class KeepFrameAllocator
{
private:
	void* mPtr;
	mallocState* m_State;
public:
	KeepFrameAllocator(mallocState* pState = NULL)
	{
		mPtr = NULL;

		if(pState)
		{
			m_State = pState;
		}
		else
		{
			m_State = tlsFrameAllocator.getmallocState();
		}
	}
	~KeepFrameAllocator()
	{
		if(mPtr  && m_State) m_State->_free(mPtr);

		mPtr = NULL;
	}
	void _free()
	{
		if(mPtr  && m_State)
		{
			m_State->_free(mPtr);
		}

		mPtr = NULL;
	}
	void* alloc(size_t size)
	{
		if(m_State)
		{
			if(mPtr)
			{
				m_State->_free(mPtr);
			}

			mPtr = m_State->alloc(size);
			return mPtr;
		}
		else
		{
			mPtr = NULL;
			return mPtr;
		}
	}
};


uint32_t getMaxFrameAllocation();

#endif//_FRAMEALLOCATOR_H__
