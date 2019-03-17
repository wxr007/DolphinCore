#ifndef _TIME_EX_H__
#define _TIME_EX_H__


#include "define/define.h"
#include <time.h>
#include <stdio.h>
/*************************************************************/
class CTimer
{
private:
	enum TimerStatus { tsRun, tsStop, tsPause };
public:
	CTimer(bool bPlay = false, bool boRDTSC = true);
	~CTimer(void);

public:
	uint32_t GetTime(int nPrecision = 1000);
	float GetTimef(float fPrecision = 1000.0);

	void Play(void);

	void Stop(void);

	void Pause(void);

	void Reset(void);
private:

	int64_t GetCurrentCount(void);

private:
	static int64_t m_n64Freq;
	int64_t m_n64TimeBegin;
	int64_t m_n64TimeEnd;
	TimerStatus m_TimerStatus;
	bool m_boRDTSC;
};

extern CTimer g_timer;

//GetTimeSec/GetTimeSecEx取的时间是相对时间,使用要注意.
time_t GetTimeSec();

time_t GetTimeSecEx();


int64_t	GetTimeMsecs();

int64_t	GetTimeMsecsEx();

int64_t	GetTimeMsecsInZone();

#define xtimeGetTime timeGetTime

#endif //_TIME_EX_H__
