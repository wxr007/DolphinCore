#include "utils/time/TimeManage.h"
#include <time.h>

uint32_t g_dwGlobalStartTime = 0;
uint32_t g_dwGlobalInitTime = 0;


uint32_t GetTime_7cool()
{
	uint32_t currentTime = 0;

 	if (g_dwGlobalStartTime == 0)
 	{
 		g_dwGlobalStartTime = time(NULL);

 		g_dwGlobalInitTime = GetUptime();
 	}

 	uint32_t dwEndtime = GetUptime();

 	currentTime = g_dwGlobalStartTime + dwEndtime - g_dwGlobalInitTime;

 	return currentTime;
}

void SetTime_7cool(uint32_t dwTime)
{
 	g_dwGlobalStartTime = dwTime;

 	g_dwGlobalInitTime = GetUptime();
}
