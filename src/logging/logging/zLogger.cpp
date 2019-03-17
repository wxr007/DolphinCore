#include "logging/logging/zLogger.h"
#include "utils/function.h"
#include "thread/qthread.h"
#include "platform/platform.h"

#include <time.h>

//#include "define/define.h"

#include "utils/convert_code/convEncode.h"

//NOTE:这里的初始化 vs版的是放在 cldinitseg.cpp 里面的
zLogger::zLevel zLogger::zOFF("OFF", zLogger::eALL + 1, zLogger::eALL + 1, false, 0);
zLogger::zLevel zLogger::zFORCE("FORCE", zLogger::eFORCE, zLogger::eFORCE, false, 0x00ff0000);

zLogger::zLevel zLogger::zFATAL("FATAL", zLogger::eFATAL, zLogger::eFATAL, true, 0x000000ff);

zLogger::zLevel zLogger::zERROR("ERROR", zLogger::eERROR, zLogger::eERROR, false, 0x000000ff);
zLogger::zLevel zLogger::zALARM("ALARM", zLogger::eALARM, zLogger::eALARM, false, 0x000000ff);

zLogger::zLevel zLogger::zWARN("WARN", zLogger::eWARN, zLogger::eWARN, false, 0x000000ff);
zLogger::zLevel zLogger::zIFFY("IFFY", zLogger::eIFFY, zLogger::eIFFY, false, 0);
zLogger::zLevel zLogger::zINFO("INFO", zLogger::eINFO, zLogger::eINFO, false, 0);
zLogger::zLevel zLogger::zTRACE("TRACE", zLogger::eTRACE, zLogger::eTRACE, false, 0);
zLogger::zLevel zLogger::zDEBUG("DEBUG", zLogger::eDEBUG, zLogger::eDEBUG, false, 0);
zLogger::zLevel zLogger::zGBUG("GBUG", zLogger::eGBUG, zLogger::eGBUG, false, 0);

extern void syslog_q(const char *log);
extern void syslogl0_q(const char *log);


/*************************************************************/
class stUpdatLogFileThread
{
public:
    stUpdatLogFileThread()
        :m_logthread(QCOOL::bind(stUpdatLogFileThread::Run),"logger")
    {
    }
    ~stUpdatLogFileThread()
    {
        save();
        m_logthread.Join();
        INFOLOCK(zLogger::m_loggers);
        zLogger::m_loggers.clear();
        UNINFOLOCK(zLogger::m_loggers);
    }
    void Start(bool,void*)
    {
        m_logthread.Start();
    }
    static void Run();
    static bool save();
private:
    QThread m_logthread;
};

stUpdatLogFileThread UpdatLogFileThread;

zLogger::zLogger(const std::string &name)
{
	m_ShowLogFunc = NULL;
	m_nShowLvl = 2;
	m_nWriteLvl = -1;
	m_nLogidx = 0;
	m_ncurpos = 0;
	m_nlogbytes = 0x7fffffff;
	m_msgbuf[m_ncurpos] = 0;
	m_basefilepath = "";
	m_name = "";
	m_nConvEnCoding = 0;
	m_zoneid = 0;
	m_svr_name = "";

	if(name != "")
	{
		m_basefilepath = "./log/";
		m_basefilepath += name;
		m_basefilepath += "/";
		setName(name);
	}

	AILOCKT(zLogger::m_loggers);
	m_loggers.push_back(this);
}

zLogger::~zLogger()
{
    //NOTE:源代码这里有未 全局变量析构顺序 问题bug
	AILOCKT(m_loggers);
	UpdatLogFile();
	m_loggers.erase(remove(m_loggers.begin(), m_loggers.end(), this), m_loggers.end());
	m_ShowLogFunc = NULL;
}

void zLogger::ShowLog(zLevel& level, const char* logtime, const char* pszMsg)
{

	if(pszMsg && m_ShowLogFunc)
	{
		m_ShowLogFunc(level, logtime, (char *)pszMsg);
	}
}

const std::string & zLogger::getName()
{
	return m_name;
}

void zLogger::setName(const std::string & setName)
{
	m_name = setName;
}

int zLogger::strlevltoint(const std::string & level)
{
	if(stricmp_q("off", level.c_str()) == 0)
		return zLogger::eOFF;
	else if(stricmp_q("fatal", level.c_str()) == 0)
		return zLogger::eFATAL;
	else if(stricmp_q("alarm", level.c_str()) == 0)
		return zLogger::eALARM;
	else if(stricmp_q("error", level.c_str()) == 0)
		return zLogger::eERROR;
	else if(stricmp_q("iffy", level.c_str()) == 0)
		return zLogger::eIFFY;
	else if(stricmp_q("warn", level.c_str()) == 0 || stricmp_q("warning", level.c_str()) == 0)
		return zLogger::eWARN;
	else if(stricmp_q("trace", level.c_str()) == 0)
		return zLogger::eTRACE;
	else if(stricmp_q("info", level.c_str()) == 0)
		return zLogger::eINFO;
	else if(stricmp_q("gbug", level.c_str()) == 0)
		return zLogger::eGBUG;
	else if(stricmp_q("debug", level.c_str()) == 0)
		return zLogger::eDEBUG;
	else if(stricmp_q("all", level.c_str()) == 0 || stricmp_q("always", level.c_str()) == 0)
		return zLogger::eALL;
	else
	{
		return atoi(level.c_str());
	}
}

void zLogger::setLevel(int writelevel, int showlvl)
{
	m_nWriteLvl = writelevel;
	m_nShowLvl = showlvl;
}

void zLogger::setLevel(int writelevel, int showlvl, int nConvEnCoding)
{
	m_nWriteLvl = writelevel;
	m_nShowLvl	= showlvl;
	m_nConvEnCoding = nConvEnCoding;
}

void zLogger::setLevel(const std::string & writelevel, const std::string & showlvl)
{
	m_nWriteLvl = strlevltoint(writelevel);
	m_nShowLvl = strlevltoint(showlvl);
}

void zLogger::fixlogpath(std::string &basepath)
{
	char szSvridxPath[MAX_PATH] = {0};
	strncpy_q(szSvridxPath, basepath.c_str(), sizeof(szSvridxPath));
	size_t nPathLen = strlen(szSvridxPath);

	if(nPathLen > 0)
	{
		replaceFrontlashPath(szSvridxPath);

		if(szSvridxPath[nPathLen-1] == '/')
		{
			szSvridxPath[nPathLen-1] = '\0';
			nPathLen--;
		}

		basepath = szSvridxPath;
	}
}

bool zLogger::SetLocalFileBasePath(const std::string &basefilepath)
{
	m_nlogbytes = 0x7fffffff;
	m_basefilepath = basefilepath;
	fixlogpath(m_basefilepath);
	UpdatLogFileThread.Start(false, NULL);//WARN 这里启动 可能会造成启动两次
	return CheckLogPath();
}

const int TEMPBUFSIZE = 1024 * 10;


#define  _ZLOGGER_TIMEFORMAT_	"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d"
#define  _ZLOGGER_TIMELEN_		20

bool zLogger::logbylevel(zLevel& level, const char *tempmessage)
{


	if(tempmessage && (level.showlevel <= m_nShowLvl || level.writelevel <= m_nWriteLvl))
	{
		char message[TEMPBUFSIZE] = {0};
		char szTime[32] = { 0 };
		timetostr(time(NULL), szTime, _ZLOGGER_TIMELEN_ , _ZLOGGER_TIMEFORMAT_);
		sprintf_q(message, sizeof(message), "[%s|%.8s|%.8s][%d|%s|%s][%s|%s][msg=%s]\r\n", szTime, level.name, project_name.c_str(), m_zoneid, m_svr_id.c_str(), m_svr_name.c_str(), "game", "log", tempmessage);

		if(level.showlevel <= m_nShowLvl)
		{
			ShowLog(level, message, &message[_ZLOGGER_TIMELEN_]);
		}

		if(level.writelevel <= m_nWriteLvl)
		{
//			message[_ZLOGGER_TIMELEN_-1] = '\x20';

			if(level.realtimewrite)
			{
				WriteLog(message, strlen(message));
			}
			else
			{
				AddMsg2buf(message, strlen(message));
			}
		}

		return true;
	}

	return false;
}

bool zLogger::log(zLevel& level, const char * pattern, ...)
{

	if(pattern && (level.showlevel <= m_nShowLvl || level.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(level, tempmessage);
		return true;
	}

	return false;
}

bool zLogger::realtimeLog(zLevel& level, const char * pattern, ...)
{

	if(pattern && (level.showlevel <= m_nShowLvl || m_nWriteLvl >= 0))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);

		char message[TEMPBUFSIZE] = {0};
		char szTime[32] = { 0 };
		timetostr(time(NULL), szTime, _ZLOGGER_TIMELEN_, _ZLOGGER_TIMEFORMAT_);		
		sprintf_q(message, sizeof(message), "[%s|%.8s|%.8s][%d|%s|%s][%s|%s][msg=%s]\r\n", szTime, level.name, project_name.c_str(), m_zoneid, m_svr_id.c_str(), m_svr_name.c_str(), "game", "log", tempmessage);

		if(level.showlevel <= m_nShowLvl)
		{
			ShowLog(level, message, &message[_ZLOGGER_TIMELEN_]);
		}

		if(m_nWriteLvl >= 0)
		{
//			message[_ZLOGGER_TIMELEN_-1] = '\x20';
			WriteLog(message, strlen(message));
		}

		return true;
	}

	return false;
}

bool zLogger::forceLog(zLevel& level, const char * pattern, ...)
{

	if(pattern && (m_nShowLvl >= 0 || m_nWriteLvl >= 0))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);

		char message[TEMPBUFSIZE] = {0};
		char szTime[32] = { 0 };
		timetostr(time(NULL), szTime, _ZLOGGER_TIMELEN_, _ZLOGGER_TIMEFORMAT_);
		sprintf_q(message, sizeof(message), "[%s|%.8s|%.8s][%d|%s|%s][%s|%s][msg=%s]\r\n", szTime, level.name, project_name.c_str(), m_zoneid, m_svr_id.c_str(), m_svr_name.c_str(), "game", "log", tempmessage);

		if(m_nShowLvl >= 0)
		{
			ShowLog(zFORCE, message, &message[_ZLOGGER_TIMELEN_]);
		}

		if(m_nWriteLvl >= 0)
		{
//			message[_ZLOGGER_TIMELEN_-1] = '\x20';

//			if(level.realtimewrite)
//			{
				WriteLog(message, strlen(message));
//			}
//			else
//			{
//				AddMsg2buf(message, strlen(message));
//			}
		}

		return true;
	}

	return false;
}

bool zLogger::debug(const char * pattern, ...)
{

	if(pattern && (zDEBUG.showlevel <= m_nShowLvl || zDEBUG.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zDEBUG, tempmessage);
		return true;
	}

	return false;
}
bool zLogger::error(const char * pattern, ...)
{

	if(pattern && (zERROR.showlevel <= m_nShowLvl || zERROR.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zERROR, tempmessage);
		return true;
	}

	return false;
}
bool zLogger::error_out(const char * pattern)
{

	if(pattern && (zERROR.showlevel <= m_nShowLvl || zERROR.writelevel <= m_nWriteLvl))
	{
		logbylevel(zERROR, pattern);
		return true;
	}

	return false;
}
bool zLogger::info(const char * pattern, ...)
{

	if(pattern && (zINFO.showlevel <= m_nShowLvl || zINFO.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zINFO, tempmessage);
		return true;
	}

	return false;
}
bool zLogger::fatal(const char * pattern, ...)
{

	if(pattern && (zFATAL.showlevel <= m_nShowLvl || zFATAL.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zFATAL, tempmessage);
		return true;
	}

	return false;
}
bool zLogger::warn(const char * pattern, ...)
{

	if(pattern && (zWARN.showlevel <= m_nShowLvl || zWARN.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zWARN, tempmessage);
		return true;
	}

	return false;
}
bool zLogger::alarm(const char * pattern, ...)
{

	if(pattern && (zALARM.showlevel <= m_nShowLvl || zALARM.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zALARM, tempmessage);
		return true;
	}

	return false;
}
bool zLogger::iffy(const char * pattern, ...)
{

	if(pattern && (zIFFY.showlevel <= m_nShowLvl || zIFFY.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zIFFY, tempmessage);
		return true;
	}

	return false;
}
bool zLogger::trace(const char * pattern, ...)
{

	if(pattern && (zTRACE.showlevel <= m_nShowLvl || zTRACE.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zTRACE, tempmessage);
		return true;
	}

	return false;
}
bool zLogger::gbug(const char * pattern, ...)
{

	if(pattern && (zGBUG.showlevel <= m_nShowLvl || zGBUG.writelevel <= m_nWriteLvl))
	{
		char tempmessage[TEMPBUFSIZE] = {0};
		va_list ap;
		va_start(ap, pattern);
		_safe_vsnprintf(tempmessage, (sizeof(tempmessage)) - 32, pattern, ap);
		va_end(ap);
		logbylevel(zGBUG, tempmessage);
		return true;
	}

	return false;
}

bool zLogger::WriteLog(char* pmsg, int nlen)
{
	AddMsg2buf(pmsg, nlen);
	UpdatLogFile();
// 	syslogl0_q(pmsg);
	return true;
}

bool zLogger::CheckLogPath()
{

	if(m_nlogbytes >= (MSGBUF_MAX * 2 - 1024))
	{
		char filepath[MAX_PATH] = {0};
		strncpy_q(filepath, m_basefilepath.c_str(), sizeof(filepath));
		size_t nPathLen = strlen(filepath);

		if(nPathLen == 0)
		{
			return false;
		}

		replaceFrontlashPath(filepath);

		if(!(filepath[nPathLen-1] == '/'))
		{
			filepath[nPathLen] = '/';
			nPathLen++;
		}

		time_t ti = time(NULL);
		tm* t = localtime(&ti);
		sprintf_q(&filepath[nPathLen], sizeof(filepath) - nPathLen, "%.4d%.2d%.2d//\0", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
		char szfile[MAX_PATH] = {0};
		sprintf_q(szfile, sizeof(szfile), "%s%.2d%.2d%.2d.log\0", filepath, t->tm_hour, t->tm_min, t->tm_sec);
		m_logout.SetFileName(szfile);//文件名:		时分秒.log
		FileSystem::createPath(filepath);
		m_nlogbytes = 0;
		return true;
	}
	else if(!FileSystem::IsFileExist(m_logout.GetFileName()))
	{
		FileSystem::createPath(extractfilepath(m_logout.GetFileName()));
	}

	return false;
}
bool zLogger::UpdatLogFile()
{
	AILOCKT(n_logoutlock);

	if(m_ncurpos > 0)
	{
		CheckLogPath();
		m_nlogbytes += m_ncurpos;
		m_logout.WriteString(m_msgbuf);
		InterlockedIncrement_Q(&m_nLogidx);
		m_ncurpos = 0;
		m_msgbuf[m_ncurpos] = 0;
		return true;
	}

	return false;
}
bool zLogger::UpdatLogNoLockFile()
{

	if(m_ncurpos > 0)
	{
		CheckLogPath();
		m_nlogbytes += m_ncurpos;
		m_logout.WriteString(m_msgbuf);
		InterlockedIncrement_Q(&m_nLogidx);
		m_ncurpos = 0;
		m_msgbuf[m_ncurpos] = 0;
		return true;
	}

	return false;
}

void zLogger::MsgConvEnCodingToUTF8(char* pmsg, int& nlen, bool boChange)
{
	if (boChange)
	{
		int  nChangeLent  = 0;
		char szSend[TEMPBUFSIZE] = { 0 };

		nChangeLent = GBKToUTF8(pmsg, szSend, sizeof(szSend));
		if (nChangeLent == -1)
		{
			g_logger.forceLog(zLogger::zERROR, "Log 数据GBK转码UTF-8失败！！！");
		}
		else if (nChangeLent < TEMPBUFSIZE)
		{
			memset(pmsg, 0, nlen);
			nlen = nChangeLent;
			sprintf_q(pmsg, nChangeLent + 1, "%s", szSend);
		}
	}
}

bool zLogger::AddMsg2buf(char* pmsg, int nlen)
{
    //syslog;
    syslogl0_q(pmsg);
/*
	if(nlen < 1024 * 8)
	{
		AILOCKT(n_logoutlock);

		//MsgConvEnCodingToUTF8(pmsg, nlen, m_nConvEnCoding);

		int n = m_ncurpos + nlen;

		if(n >= (MSGBUF_MAX - 1024))
		{
		    //NOTE:源代码这里有死锁bug
			UpdatLogNoLockFile();
			n = m_ncurpos + nlen;

			if(n >= (MSGBUF_MAX - 1024))
			{
				char message[TEMPBUFSIZE] = {0};
				timetostr(time(NULL), message, _ZLOGGER_TIMELEN_, _ZLOGGER_TIMEFORMAT_);
				message[_ZLOGGER_TIMELEN_-1] = 0;
				sprintf_q(&message[_ZLOGGER_TIMELEN_], sizeof(message) - 32, "[ Error:日志缓冲区不足(%u : %u : %u) ] %s\r\n", m_ncurpos, nlen, m_nlogbytes, pmsg);
				zLogger::zLevel tempFATAL("FATAL", zLogger::eFATAL, zLogger::eFATAL, true);
				ShowLog(tempFATAL, message, &message[_ZLOGGER_TIMELEN_]);
//				message[_ZLOGGER_TIMELEN_-1] = '\x20';
				m_logout.WriteString(message);
				return false;
			}
		}

		memcpy(&m_msgbuf[m_ncurpos], pmsg, nlen);
		m_ncurpos += nlen;
		m_msgbuf[m_ncurpos] = 0;
		return true;
	}*/

	return false;
}

bool zLogger::save()
{
	return stUpdatLogFileThread::save();
}

void zLogger::setSvrInfo( int zoneid,const std::string& svr_id,const std::string& svr_name,const std::string& project)
{
	m_zoneid = zoneid;
	m_svr_id = svr_id;
	m_svr_name = svr_name;
	project_name = project;
}

bool stUpdatLogFileThread::save()
{
	CSyncVector<zLogger*> m_templist;
	INFOLOCK(zLogger::m_loggers);
	CSyncVector<zLogger*>::iterator it;

	for(it = zLogger::m_loggers.begin(); it != zLogger::m_loggers.end(); ++it)
	{
		if((*it) != NULL)
		{
			m_templist.push_back(*it);
		}
	}

	UNINFOLOCK(zLogger::m_loggers);

	for(it = m_templist.begin(); it != m_templist.end(); ++it)
	{
		(*it)->UpdatLogFile();
	}

	return true;
}

void stUpdatLogFileThread::Run()
{
	time_t dwRunTime = 0;
	//SetPriority(THREAD_PRIORITY_IDLE);
	CSyncVector<zLogger*> m_templist;

	while(true)
	{
		INFOLOCK(zLogger::m_loggers);
		CSyncVector<zLogger*>::iterator it;
		if(zLogger::m_loggers.size() > 0)
		{
			if(time(NULL) > dwRunTime)
			{
				dwRunTime = time(NULL) + 3;

				for(it = zLogger::m_loggers.begin(); it != zLogger::m_loggers.end(); ++it)
				{

					if(Random_Q(10) <= 2)
					{
						if((*it) != NULL)
						{
							m_templist.push_back(*it);
						}
					}
				}
			}
		}

		UNINFOLOCK(zLogger::m_loggers);

		if(m_templist.size() > 0)
		{
			for(it = m_templist.begin(); it != m_templist.end(); ++it)
			{
				(*it)->UpdatLogFile();
			}
		}

		m_templist.clear();
		Sleep_Q(1000);
	}

	save();
}

