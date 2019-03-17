#ifndef _SQL_HELPER_H__
#define _SQL_HELPER_H__

#define OPENMYSQL_H_INCLUDED
#ifdef OPENMYSQL_H_INCLUDED

//mysql 数据库sqlbase实现

#include "platform/platform.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sql/sqlDataDefine.h"
#include "thread/lockObj.h"
#include <iostream>
#include <sstream>
#include "utils/string/stringex.h"
#include <zconf.h>
#include "compress/zlib/compress.h"
#include <stdarg.h>
#include "mysql.h"
#include "thread/qthread.h"
#include "mem/loopBuf.h"
#include "thread/condSigal.h"



#define ERR_MSG_LEN 1024

#define	CONVERT_BIN_BEGIN		"CONVERT(varbinary(%d),"
#define	CONVERT_BIN_BEGIN_MYSQL	"CONVERT(%s,binary(%d))"
#define	CONVERT_BIN_END			")"


#ifndef STRUCT_COL
#define STRUCT_COL
struct COL
{
    char *name;
    char *buffer;
    int type, size, status;
    dbCol *m_dbCol;
};

#define SQL_UTF8 "UTF-8"

#define SQL_GBK  "GBK"

#define MAX_SQL_SELECTLEN   1024*2



#define MAX_INFOSTR_SIZE	128

class CSqlBase;
typedef CSqlBase CSqlClientHandle;


class stUrlInfo
{
public:
	typedef  CSqlClientHandle*(* pCreateSqlClientHandle)(stUrlInfo* ui);

public:
	enum eSqlType
	{
		eTypeError = 0,

		eMsSql = 2,
		eMsMdb = 3,
		eMsXls = 4,
		eMySql = 5,		
	};
	struct stSqlTypeInfo
	{
		const char* head;
		eSqlType type;
		uint8_t maxhandles;
		pCreateSqlClientHandle pnew;
	};

public:
	static stSqlTypeInfo zSqlTypes[];

	const unsigned int hashcode;
	const std::string url;
	const bool supportTransactions;
	const unsigned int poolMax;

public:
	stUrlInfo()
		: hashcode(0), url(), supportTransactions(false),poolMax(1024), sqltypeinfo(NULL), urlerror(false),maxHandleBuf(0) {};

	stUrlInfo(const unsigned int hashcode, const std::string &parurl, const bool _supportTransactions, uint8_t maxHandle = 0,const unsigned int poolMax = 1024)
		: hashcode(hashcode), url(parurl), supportTransactions(_supportTransactions), poolMax(poolMax),sqltypeinfo(NULL), urlerror(false), maxHandleBuf(maxHandle)
	{
		parseURLString();

		if(maxHandleBuf <= 0)
		{
			if(sqltypeinfo)
			{
				maxHandleBuf = sqltypeinfo->maxhandles;
			}
			else
			{
				maxHandleBuf = 1;
			}
		}
	}
	stUrlInfo(stUrlInfo* ui)
		: hashcode(ui->hashcode), url(ui->url),  supportTransactions(ui->supportTransactions),poolMax(ui->poolMax), sqltypeinfo(NULL),urlerror(ui->urlerror)
	{
		strncpy_q(host, ui->gethost(), sizeof(host));
		strncpy_q(user, ui->getuser(), sizeof(user));
		strncpy_q(passwd, ui->getpasswd(), sizeof(passwd));
		strncpy_q(dbConnParam, ui->getparam(), sizeof(dbConnParam));
		port = ui->getport();
		strncpy_q(dbName, ui->getdbName(), sizeof(dbName));
		sqltypeinfo = ui->gettypeinfo();
		maxHandleBuf = ui->getmaxhandlebuf();

		if(maxHandleBuf <= 0)
		{
			if(sqltypeinfo)
			{
				maxHandleBuf = sqltypeinfo->maxhandles;
			}
			else
			{
				maxHandleBuf = 1;
			}
		}
	}

public:
	CSqlClientHandle* newsqlclienthandle()
	{
		if(this != NULL && sqltypeinfo && sqltypeinfo->pnew)
		{
			return sqltypeinfo->pnew(this);
		}

		return NULL;
	}
	char* gethost()
	{
		return host;
	}
	char* getuser()
	{
		return user;
	}
	char* getpasswd()
	{
		return passwd;
	}
	char* getparam()
	{
		return dbConnParam;
	}
	unsigned int getport()
	{
		return port;
	}
	char* getdbName()
	{
		return dbName;
	}
	stSqlTypeInfo* gettypeinfo()
	{
		return sqltypeinfo;
	}
	bool geturlerror()
	{
		return urlerror;
	}
	int getmaxhandlebuf()
	{
		return (int)maxHandleBuf;
	}

	eSqlType gettype()
	{
		return sqltypeinfo == NULL ? eTypeError : sqltypeinfo->type;
	}

	static stSqlTypeInfo* gettypeinfo(eSqlType type);
protected:
	stSqlTypeInfo* sqltypeinfo;
	char host[MAX_INFOSTR_SIZE*2];
	char user[MAX_INFOSTR_SIZE];
	char passwd[MAX_INFOSTR_SIZE];
	unsigned int port;
	char dbName[MAX_INFOSTR_SIZE*4];
	char dbConnParam[MAX_INFOSTR_SIZE*8];
	bool urlerror;
	uint8_t maxHandleBuf;

	const char* parseurlhead(const char* connstr);

	void parseURLString();
};



class stUrlInfo;
#endif

#ifndef CLASS_CSQLBASE
#define CLASS_CSQLBASE

#define SQL_LEN			1024*4
struct stSqlOperation	//sql操作
{
	uint32_t dwHashCode;			//操作hash值
	dbCol* col;
	char szSqlStr[SQL_LEN];
	stSqlOperation()
	{
		ZEROSELF;
	}
};
struct stSqlResult
{
	uint32_t dwHashCode;
	dbCol* col;
	MYSQL_RES* MyResult;
	int nEffectLine;
	stSqlResult()
	{
		ZEROSELF;
	}
};
class SqlConnPool;

class CSqlBase
{
public:
	CSqlBase();
	CSqlBase(stUrlInfo* ui);
	~CSqlBase();
	static CSqlBase* newInstance(stUrlInfo* ui)
	{
		CSqlBase* sqlbase= (CSqlBase*)(new CSqlBase(ui));
		return sqlbase;
	}
	bool    ReOpenDB(const char* charset=SQL_UTF8);
	bool    bDead();
	void    ReDead();
	bool    bCoonect();                                                                 //当前是否连接上sql true 连上
	bool	ReConnect();																//重连mysql因为长期无操作mysql链接会断开(返回false表示重连失败)
	bool    openDB(const char* server, const char* user, const char* password,const char* dbname="",const char* charset=SQL_UTF8);                                       //连接数据库 server 127.0.0.1:1433 这样格式 GBK UTF-8
	bool    closeDb();                                                                  //关闭sql
	int64_t execSql(const char * sql,size_t n=0);                                       //执行insert update delete
	int64_t getaffectedrows();                                                          //执行所有sql 语句 true 成功 false 失败
	bool    writeData(const char *sql);                                                 //增删改查
	int64_t getCount(const char* tbName,const char* sql=NULL);                          //获得一个表 条件sql 的行数
	bool    escapebin2str(const char *src, size_t srcsize, char *dest);
	char*   escapeStr(const char *src, char *dest, size_t srcsize=0);
	int64_t sprintf_exec(const char * pattern, ...);
	int64_t execInsert(const char *tableName,const dbCol *column,const unsigned char *data,const char* noexists_where=NULL,const char* noexists_table=NULL,void* table=NULL);//(外部调用)
	int64_t execUpdate(const char *tableName, const dbCol *column,const unsigned char *data, const char *where, void* table = NULL);      //执行update(外部调用)
	int64_t execSelectSqlRealWork(const char * szSql,dbCol* col, unsigned char* pInData);       //获得该 sql语句的数据 并且bind到 结构体上(内部调用)
	int64_t	execSelectSql(const char *sql , const dbCol *column,unsigned char* data, unsigned int maxbuflen = 0 , unsigned int sqllen = 0);//(外部调用)
	int64_t execDelete(const char *tableName, const char *where);//(外部调用)
	//读取多个bindata数据
	int64_t exexSelectMultiBinDataSql(const char *sql ,const dbCol *column,unsigned char* data,unsigned int maxbuflen=0 ,unsigned int sqllen=0,char **ppArray = NULL);//(内部调用)
public:
	__inline const stUrlInfo& geturl()
	{
		return url;
	}

	bool initHandle()
	{
		FUNCTION_BEGIN;


		return m_bCooncet;
	}
	unsigned int getID()
	{
		return id;
	}
	void finalHandle()
	{
		return;
	}
	void unsetHandle()
	{
		return;
	}
	int getinsertid();

	static unsigned int getColInfoMaxStLen(const dbCol* column);									//获得 结构体的偏移信息
	static dbCol*  findDbCol(dbCol* dbColArr,const char * name,int nindex);							//根据名字找到 列 信息 dbCol
	static int putUserData(int nType,const char* psqlData, char* pIndata,const unsigned int nMaxLen);  //根据数据类型 写入 数据到结构体

private:
	//    bool    escapebin2str(const char *src, unsigned int srcsize, char *dest);
	eSD2U_RC userdata2sql(int _etype, std::ostringstream& sqlout, const unsigned char * userbuf,const size_t dwuserlen, size_t offset);
	bool	 bExecuteSql(const char* sql);                                                           //最基本的执行sql 语句 未加锁 不能直接调用
	uint64_t extInsertUpdate(const char *sql);
private:
	CIntLock  m_sqlLock;//lock
	int64_t   m_naffectedrows;//返回 insert update delete 执行后 受影响的行数
	bool	  m_bexesqlResult;//执行sql的结果
	MYSQL*	  m_mysql;
	char	  errMsg[ERR_MSG_LEN];
	bool      m_bCooncet;
private:
	static int64_t HandleID_generator;
	const unsigned int id;
	stUrlInfo url;
	//断线重连
	bool m_bsaveonce;
	char m_hostip[MAX_INFOSTR_SIZE*2];
	char m_username[MAX_INFOSTR_SIZE];
	char m_passwd[MAX_INFOSTR_SIZE];
	char m_dbName[MAX_INFOSTR_SIZE*4];
protected:
	//执行任意sql语句后初始化  m_qinid=-1 和 m_bocangetqinid=false
	unsigned int m_qinid;
	bool m_bocangetqinid;
public:
	CLoopList<stSqlOperation>			m_sqllist;											//操作集
	int m_Time;							//时间
	int n_Count;						//操作数
	time_t m_NowTime;					//当前时间

	QThread*						m_pSqlThread;										//sql处理线程
	uint32_t								m_RunTickTime;										//执行时间
	SqlConnPool*							m_MyPool;											//所属连接池
	CCondSigal							m_ProcessCondLock;									//信号量
	bool								m_Active;											//是否活跃												
	unsigned int  SqlProcessThread();						//处理sql消息线程
	void setMyPool(SqlConnPool*	Pool);					
};
#endif

#endif

#endif //_SQL_HELPER_H__
