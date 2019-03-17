/*------------- frameAllocator.cpp
*
* Copyright (C): www.7cool.cn (2010)
* Author       :
* Version      : V1.01
* Date         : 2010/11/25 17:55:35
*
*/
/*************************************************************
*
*************************************************************/
#include "mem/frameallocator.h"
/*************************************************************/


void mallocState::Init(char *beg, char *end)
{
	m_beg = beg;
	m_end = end;
	m_last = NULL;
	m_nMaxSize = m_end - m_beg;
	m_cur = m_beg;
	m_nMaxFrameAllocation = 0;
}

void mallocState::Init(char *beg, int nSize)
{
	m_beg = beg;
	m_nMaxSize = nSize;
	m_last = NULL;
	m_end = m_beg + m_nMaxSize;
	m_cur = m_beg;
	m_nMaxFrameAllocation = 0;
}

void mallocState::Init(mallocState* other)
{
	m_beg = other->getbeg();
	m_nMaxSize = other->getBufferSize();
	m_end = m_beg + m_nMaxSize;
	m_cur = m_beg + other->getCurSize();
	m_last = other->m_last;
	m_nMaxFrameAllocation = other->getUseMaxSize();
}

void mallocState::Uninit()
{
	g_logger.debug("mallocState log-->[%d/%d]", getUseMaxSize(), getBufferSize());
	m_beg = 0;
	m_end = 0;
	m_nMaxSize = 0;
	m_cur = 0;
	m_last = NULL;
	m_nMaxFrameAllocation = 0;
}


mallocState::mallocState()
{
	Uninit();
}
mallocState::mallocState(char *beg, char *end)
{
	Init(beg, end);
}
mallocState::mallocState(char *beg, int nSize)
{
	Init(beg, nSize);
}
mallocState::mallocState(const mallocState& other)
{
	Init((mallocState*)&other);
}
void mallocState::reset()
{
	m_cur = m_beg;
	m_last = NULL;
	m_nMaxFrameAllocation = 0;
}
void* mallocState::alloc(uint64_t allocSize)
{
	if(m_beg && ((m_cur + allocSize) < m_end))
	{
		m_last = m_cur;
		m_cur += allocSize;
		int nCurSize = INT32(m_cur - m_beg);

		if(nCurSize > m_nMaxFrameAllocation)
			m_nMaxFrameAllocation = nCurSize;

		return m_last;
	}
	else
	{
		g_logger.forceLog(zLogger::zFATAL, "[ fatal error ] mallocState is out memory-->[%d/%d](%d) ", getCurSize(), getBufferSize(), allocSize);
		return malloc(allocSize);
	}
}

bool mallocState::issysmalloc(void* p)
{
	return (!(p >= m_beg && p < m_end));
}

void mallocState::_free(void* p)
{
	if(p >= m_beg && p < m_end) {}
	else if(p)
	{
		free(p);
	}
}


void mallocState::setCurSize(const int waterMark)
{
	if(waterMark >= 0 && waterMark < m_nMaxSize)
	{
		m_cur = m_beg + waterMark;
	}
}
int mallocState::getCurSize()
{
	return (int)(m_cur - m_beg);
}
int mallocState::getBufferSize()
{
	return m_nMaxSize;
}

int mallocState::getUseMaxSize()
{
	return m_nMaxFrameAllocation;
}

char* mallocState::getbeg()
{
	return m_beg;
};

void stStackFrameAllocator::init(uint64_t frameSize, bool bocheckthreadobj)
{
	frameSize = (frameSize >= 1024 * 256) ? (ROUNDNUM2(frameSize, 1024 * 64)) : (1024 * 1024 * 2);
	frameSize = safe_max(frameSize, m_initframeSize);
	m_currthread = GetCurrentThreadId_Q();

	if(/*CThreadBase::getThreadObj() != NULL || */!bocheckthreadobj)
	{
		if(!m_State)
		{
			m_State = (mallocState*)&Statebuf;
			constructInPlace(m_State);
			char* p = (char*)malloc(frameSize);
			assert((p != NULL) && "Error, malloc memory error");
			m_State->Init(p, INT32(frameSize));
			m_initframeSize = safe_max(frameSize, m_initframeSize);
		}
	}
	else
	{
		g_logger.forceLog(zLogger::zFATAL, "[ fatal error ] thread %d init mallocState ", m_currthread);
	}
}


uint32_t getMaxFrameAllocation()
{
	return (tlsFrameAllocator.m_State != NULL) ? tlsFrameAllocator.m_State->getUseMaxSize() : 0;
}
