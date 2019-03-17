#ifndef _CONN_POOL_H__
#define _CONN_POOL_H__

#include "platform/platform.h"
#include "sql/sqlHelper.h"
#include "utils/once_call.h"
#include "mem/loopBuf.h"
#include "thread/condSigal.h"
#include "mem/synclist.h"

class SqlConnPool							//连接池
{
private:
	stUrlInfo	m_UrlInfo;
	CIntLock	mlock;									//互斥锁
	list<CSqlClientHandle*> m_connList;					//连接池的容器队列
	list<CSqlClientHandle*> m_bakList;					//后备连接对象队列
	CLoopList<stSqlOperation> m_sqlCacheReceive;		//接收
	CLoopList<stSqlOperation> m_sqlCacheIssue;			//操作集
	CSyncMap<uint32_t,stSqlResult>		ResultMap;			//结果集
	uint32_t m_MaxCache;									//最大使用cache

	volatile int m_nSqlCountPerSapce;					//一段时间的sql数量

	friend class CSqlBase;
public:
	SqlConnPool(stUrlInfo& ui); //构造方法
	~SqlConnPool();
    void InitConnection();								//初始化数据库连接池
	void DestoryConnPool();								//销毁数据库连接池

	void AddConnectionFromBak(int nCount = 1);			//从后备列表添加链接到活跃列表
	void DelConnectionFromActive(int nCount = 1);		//从活跃列表移除链接到后备列表

	__inline const stUrlInfo& geturl()
	{
		return m_UrlInfo;
	}
	void PrintDebugAllInfo();

	int execSqlAsy(const char * sql,uint32_t dwHashCode = 0);									    //异步执行insert update delete
	int execSelectSqlAsy(const char *sql,const dbCol *column,uint32_t dwHashCode = 0);				//异步执行select

	bool AddSqlRestult(uint32_t dwHashCode,int nEffectCount,dbCol* col,MYSQL_RES* MyResult);					//添加结果集
	int GetSqlRestult(uint32_t dwHashCode,dbCol* col, unsigned char* pInData);					//返回有效行数 和 数据 返回-1错误			
	int GetEffectCount(uint32_t dwHashCode,bool boDel = false);								//获取影响行数

	QThread*						m_pCopyThread;										//ache拷贝线程
	unsigned int  SqlCacheCopyThread();					//处理Cache拷贝线程

    QThread*						m_pCheckThread;									//链接守护线程
	time_t							m_LastReConnectTime;							//上次重连时间
	unsigned int  SqlConnectCheckThread();				//处理链接守护线程
};

class SqlConnPoolManage : public QCOOL::Singleton< SqlConnPoolManage>
{
public:
	SqlConnPoolManage();
	typedef map<unsigned int,SqlConnPool*> ConnPoolList;
	ConnPoolList	m_PoolList;			//池列表
	bool addPool(stUrlInfo& ui);
	SqlConnPool*	GetPool(unsigned int nhashcode);
	void PrintDebugAllInfo(unsigned int nhashcode);
};

#endif //_CONN_POOL_H__
