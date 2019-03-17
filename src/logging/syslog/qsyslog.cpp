
#include "platform/platform.h"
#ifdef WIN32

void syslog_q(const char *log)
{
}

#else

#include <syslog.h>
#include "utils/once_call.h"
class QSyslog : public QCOOL::Singleton<QSyslog>
{
public :
    QSyslog()
    {
        openlog("game-log", LOG_PID, LOG_LOCAL0 ) ;
    }
    ~QSyslog()
    {
        closelog();
    }

    void SyslogL0(const char *log)
    {
        syslog(LOG_LOCAL0|LOG_INFO,"%s",log);
    }
	void SyslogL1(const char *log)
	{
		syslog(LOG_LOCAL1|LOG_INFO,"%s",log);
	}
};

//QSyslog g_syslog;

void syslogl0_q(const char *log)
{
	QSyslog::Instance().SyslogL0(log);
}
void syslogl1_q(const char *log)
{
	QSyslog::Instance().SyslogL1(log);
}
#endif


#ifdef _TEST_SYSLOG_
#include <stdio.h>

int main(int argc,char *argv[])
{
	#ifdef WIN32
	printf("Just can run in Linux!\n");
	#endif
	if (argc != 2)
	{
		printf("Please Set strings what you want to write syslog!\n");
		printf("Usage:syslog \"content\"\n");
		return 0;
	}
	syslogl0_q(argv[1]);
	return 0;
}
#endif






