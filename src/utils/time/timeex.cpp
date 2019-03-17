#include "platform/platform.h"
#include "utils/time/timeex.h"
#include "utils/time/TimeManage.h"

CTimer g_timer(true);

int64_t CTimer::m_n64Freq = 0;


CTimer::CTimer(bool bPlay, bool boRDTSC)
	: m_n64TimeBegin(0)
	, m_n64TimeEnd(0)
	, m_TimerStatus(tsStop)
	, m_boRDTSC(boRDTSC)
{
//	FUNCTION_BEGIN;
//
//	if(m_boRDTSC)
//	{
//		if(m_n64Freq == 0)
//		{
//			LARGE_INTEGER tmp;
//
//			if(QueryPerformanceFrequency(&tmp) == false)
//			{
//				m_boRDTSC = false;
//			}
//			else
//			{
//				m_n64Freq = tmp.QuadPart;
//			}
//		}
//	}
//
//	if(!m_boRDTSC)
//	{
//		m_n64Freq = 1000;
//	}
//
//	if(bPlay)
//	{
//		Play();
//	}
}







CTimer::~CTimer(void)
{
//	FUNCTION_BEGIN;
//
//	if(m_TimerStatus != tsStop)
//	{
//		Stop();
//	}

}
#if 0
uint32_t CTimer::GetTime(int nPrecision)
{
	if(nPrecision < 1)
	{
		nPrecision = 1;
	}

	if(m_TimerStatus != tsRun)
	{
		if(m_n64TimeEnd < m_n64TimeBegin)
		{
			m_n64TimeEnd = m_n64TimeBegin;
		}

		if(nPrecision != m_n64Freq)
		{
			return uint32_t((m_n64TimeEnd - m_n64TimeBegin) * nPrecision / m_n64Freq);
		}
		else
		{
			return uint32_t(m_n64TimeEnd - m_n64TimeBegin);
		}
	}
	else
	{
		if(nPrecision != m_n64Freq)
		{
			return uint32_t((GetCurrentCount() - m_n64TimeBegin) * nPrecision / m_n64Freq);
		}
		else
		{
			return uint32_t(GetCurrentCount() - m_n64TimeBegin);
		}
	}
}

float CTimer::GetTimef(float fPrecision)
{
	if(fPrecision < 1.0)
	{
		fPrecision = 1.0;
	}

	if(m_TimerStatus != tsRun)
	{
		if(m_n64TimeEnd < m_n64TimeBegin)
		{
			m_n64TimeEnd = m_n64TimeBegin;
		}

		return (float)(((double)(m_n64TimeEnd - m_n64TimeBegin)) * fPrecision / (double)m_n64Freq);
	}
	else
	{
		return (float)(((double)(GetCurrentCount() - m_n64TimeBegin)) * fPrecision / (double)m_n64Freq);
	}
}



void CTimer::Reset(void)
{
	m_TimerStatus = tsRun;
	m_n64TimeBegin = GetCurrentCount();
}






void CTimer::Play(void)
{
	if(m_TimerStatus == tsStop)
	{
		m_n64TimeBegin = GetCurrentCount();
	}

	m_TimerStatus = tsRun;
}







void CTimer::Stop(void)
{
	m_n64TimeEnd = GetCurrentCount();
	m_TimerStatus = tsStop;
}







void CTimer::Pause(void)
{
	m_n64TimeEnd = GetCurrentCount();
	m_TimerStatus = tsPause;
}
int64_t CTimer::GetCurrentCount(void)
{
	if(m_boRDTSC)
	{
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		return tmp.QuadPart;
	}
	else
	{
		return timeGetTime_Q();
	}
}
#endif

time_t GetTimeSec()
{
	//return time(NULL);
	return GetTime_7cool();
}

time_t GetTimeSecEx()
{
//	static time_t dwStartsec = GetTimeSec();
//	static int64_t dwStartTickCount = GetTickCount64_Q();
//	return dwStartsec + (time_t)((GetTickCount64_Q() - dwStartTickCount) / 1000);
	return GetTime_7cool();
}


int64_t	GetTimeMsecs()
{
    return GetTickCount64_Q();
}

int64_t	GetTimeMsecsInZone()
{
// 	long long currentTime = 0;
// 
// 	struct timeval current;
// 	struct timezone zoneTime;
// 	gettimeofday(&current, &zoneTime);
// 
// 	//林威治时间往西方的分钟时差 ,例东8区 结果为-480 分钟
// 	int minut = zoneTime.tz_minuteswest;

// 
// 	currentTime = (long long)current.tv_sec * 1000;
// 	currentTime += current.tv_usec / 1000;
// 	currentTime -= minut * 60 * 1000;
// 
// 	return currentTime;
    return GetTickCount64_Q();
}


int64_t	GetTimeMsecsEx()
{
	static int64_t	 i64StartMsecs = GetTimeMsecs();
	static int64_t	 dwStartTickCount = GetTickCount64_Q();
	return i64StartMsecs + (GetTickCount64_Q() - dwStartTickCount);
}
