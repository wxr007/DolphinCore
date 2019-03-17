#ifndef _ZLOGGER_H__
#define _ZLOGGER_H__

#include "logging/filelog/output.h"
#include "define/noncopyable.h"
#include <string>
//#include"define/define.h"

// #include "ThreadFactor/Cthread.h"
//#include "filemapping.h"
#include "mem/synclist.h"
#include <string.h>
#include"utils/string/stringex.h"
#include"logging/filelog/winFileio.h"




#define MSGBUF_MAX	64*1024

using namespace std;



class zLogger
{
public:



	enum eLoggerLevel
	{
		eOFF = 0,
		eFORCE = 0,
		eFATAL = 1,
		eERROR = 2,
		eALARM = 2,
		eWARN = 3,
		eIFFY = 3,
		eINFO = 4,
		eTRACE = 4,
		eDEBUG = 5,
		eGBUG = 5,
		eALL = 250,
	};
public:
	struct zLevel
	{
		const char* name;
		const uint8_t writelevel;
		const uint8_t showlevel;
		const bool realtimewrite;
		uint32_t showcolor;

		zLevel(const char* sname, uint8_t btwritelevel, uint8_t btshowlevel, bool borealtimewrite, uint32_t dwshowcolor = 0)
			: name(sname), writelevel(btwritelevel),showlevel(btshowlevel), realtimewrite(borealtimewrite),
			 showcolor(dwshowcolor)

		{
		}
	};

	typedef void( *pShowLogFunc)(zLevel& level, const char* logtime, const char* pszMsg);

	static zLevel zOFF;
	static zLevel zFORCE;
	static zLevel zFATAL;
	static zLevel zERROR;
	static zLevel zALARM;
	static zLevel zWARN;
	static zLevel zIFFY;
	static zLevel zINFO;
	static zLevel zTRACE;
	static zLevel zDEBUG;
	static zLevel zGBUG;


	zLogger(const std::string & name = "_Logger");
	virtual ~zLogger();

	virtual void ShowLog(zLevel& level, const char* logtime, const char* pszMsg);

	const std::string & getName();
	void setName(const std::string & setName);

	virtual void setLevel(const std::string & writelevel, const std::string & showlvl = "6");
	virtual void setLevel(int writelevel, int showlvl = 6);
	virtual void setLevel(int writelevel, int showlvl,int nConvEnCoding);
	virtual bool SetLocalFileBasePath(const std::string &basefilepath);
	virtual void setSvrInfo( int zoneid,const std::string& svr_id,const std::string& svr_name,const std::string& project);
	bool log(zLevel& level, const char * pattern, ...);

	bool forceLog(zLevel& level, const char * pattern, ...);

	bool realtimeLog(zLevel& level, const char * pattern, ...);

	bool debug(const char * pattern, ...);
	bool error(const char * pattern, ...);
	bool error_out(const char * pattern);
	bool info(const char * pattern, ...);
	bool fatal(const char * pattern, ...);
	bool warn(const char * pattern, ...);
	bool alarm(const char * pattern, ...);
	bool iffy(const char * pattern, ...);
	bool trace(const char * pattern, ...);
	bool gbug(const char * pattern, ...);

	virtual void SetShowLogFunc(pShowLogFunc pfunc)
	{
		m_ShowLogFunc = pfunc;
	};

	int showlvl()
	{
		return m_nShowLvl;
	}
	int writelvl()
	{
		return m_nWriteLvl;
	}

	bool logbylevel(zLevel& level, const char *tempmessage);
protected:
	friend class stUpdatLogFileThread;

	virtual void fixlogpath(std::string &basepath);
	virtual bool WriteLog(char* pmsg, int nlen);
	virtual bool AddMsg2buf(char* pmsg, int nlen);
	virtual bool UpdatLogFile();//加锁进行保存
    bool UpdatLogNoLockFile();//不加锁进行保存
	virtual bool CheckLogPath();

	virtual int strlevltoint(const std::string & level);

	void MsgConvEnCodingToUTF8(char* pmsg, int& nlen, bool boChange = false);
private:
	pShowLogFunc m_ShowLogFunc;

	char m_msgbuf[MSGBUF_MAX];
	int m_ncurpos;

	int m_nShowLvl;
	int m_nWriteLvl;
	int m_nlogbytes;

	std::string m_name;
	std::string m_basefilepath;
	COutput m_logout;
	CIntLock n_logoutlock;
	int32_t m_nLogidx;
	int32_t m_nConvEnCoding;
	int32_t m_zoneid;
	std::string m_svr_name;
	std::string m_svr_id;
	std::string project_name;

	static CSyncVector<zLogger*> m_loggers;//日志数组对象 自动添加this 指针到该数组中
public:

	static bool save();
};


extern zLogger g_logger;







#endif // _ZLOGGER_H__
