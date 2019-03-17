#include "sql/connPool.h"
#include "logging/logging/zLogger.h"
#include "mysql.h"

SqlConnPool::SqlConnPool( stUrlInfo& ui )
    :m_UrlInfo(&ui),
	m_sqlCacheReceive(ui.poolMax),
	m_sqlCacheIssue(ui.poolMax)
{
	m_connList.clear();
	m_bakList.clear();
	m_pCopyThread = NULL;
	m_pCheckThread = NULL;
	m_LastReConnectTime = 0;
	m_MaxCache = 0;
}

void SqlConnPool::InitConnection()
{
 	AILOCKT(mlock);
 	for (int i = 0; i < m_UrlInfo.getmaxhandlebuf(); i++)
	{
 		CSqlClientHandle* handle = m_UrlInfo.newsqlclienthandle();
 		if (handle)
 		{
			if (handle->m_pSqlThread)
			{
				handle->m_pSqlThread->Start();
			}
 			handle->setMyPool(this);
 			m_connList.push_back(handle);
 		}
 		else
 		{
 			g_logger.error("创建链接出错,增加数据库连接失败！");
 		}
 	}
    m_pCopyThread = new QThread(QCOOL::bind(&SqlConnPool::SqlCacheCopyThread, this), "sql_copy");
    m_pCopyThread->Start();

    m_pCheckThread = new QThread(QCOOL::bind(&SqlConnPool::SqlConnectCheckThread, this), "sql_check");
    m_pCheckThread->Start();

}

void SqlConnPool::DestoryConnPool()
{
	AILOCKT(mlock);
	if(m_connList.size() > 0)
	{
		for(list<CSqlClientHandle*>::iterator it = m_connList.begin(); it != m_connList.end(); ++it)
		{
			CSqlClientHandle *tempHandle = (CSqlClientHandle*)*it;

			if(tempHandle)
			{
				tempHandle->finalHandle();
			}
			SAFE_DELETE(tempHandle);
		}
	}
	m_connList.clear();

	if (m_bakList.size() > 0)
	{
		for(list<CSqlClientHandle*>::iterator it = m_bakList.begin(); it != m_bakList.end(); ++it)
		{
			CSqlClientHandle *tempHandle = (CSqlClientHandle*)*it;

			if(tempHandle)
			{
				tempHandle->finalHandle();
			}
			SAFE_DELETE(tempHandle);
		}
	}
	m_bakList.clear();
}

/*
bool CmpConnect(CSqlClientHandle* con1, CSqlClientHandle* con2)
{
	//true 交换 false 不交换
	if (con1 && con2)
	{
		return ( (CSqlBase*)con1->bCoonect() && !(CSqlBase*)con2->bCoonect() );//将连接的移动到前面
	}
	else
	{
		return con1 && !con2;//将连接的移动到前面
	}
}*/

void SqlConnPool::AddConnectionFromBak(int nCount)
{
//	m_bakList.sort(CmpConnect);
	while (m_bakList.size() > 0 && nCount > 0)
	{
		list<CSqlClientHandle*>::iterator it = m_bakList.begin();
		CSqlBase* Conn = (CSqlClientHandle*)*it;
		if (Conn->bCoonect())//已连接的
		{
			m_connList.push_back((CSqlClientHandle*)Conn);
			m_bakList.pop_front();

			INFOLOCK(Conn->m_ProcessCondLock.getCIntLock());
			Conn->m_Active = true;
			Conn->m_ProcessCondLock.setEvent();
			UNINFOLOCK(Conn->m_ProcessCondLock.getCIntLock());
		}
		else//未连接的
		{
			if (Conn->ReOpenDB())//重新打开
			{
				m_connList.push_back((CSqlClientHandle*)Conn);
				m_bakList.pop_front();

				INFOLOCK(Conn->m_ProcessCondLock.getCIntLock());
				Conn->m_Active = true;
				Conn->m_ProcessCondLock.setEvent();
				UNINFOLOCK(Conn->m_ProcessCondLock.getCIntLock());
			}
			else
			{
				Sleep_Q(5*1000);		//先休眠
			}
		}
		nCount--;
	}
}
/*
bool CmpUnconnect(CSqlClientHandle* con1, CSqlClientHandle* con2)
{
	//true 交换 false 不交换
	if (con1 && con2)
	{
		return ( (!(CSqlBase*)con1->bCoonect()) && (CSqlBase*)con2->bCoonect() );//将未连接的移动到前面
	}
	else
	{
		return (!con1) && con2;//将未连接的移动到前面
	}
}*/

void SqlConnPool::DelConnectionFromActive(int nCount)
{
//	m_connList.sort(CmpUnconnect);
	while (m_connList.size() > 0 && nCount > 0)
	{
		list<CSqlClientHandle*>::iterator it = m_connList.begin();
		CSqlBase* Conn = (CSqlClientHandle*)*it;
		Conn->m_Active = false;
		m_bakList.push_back((CSqlClientHandle*)Conn);
		m_connList.pop_front();
		nCount--;
	}
}

SqlConnPool::~SqlConnPool()
{
	DestoryConnPool();
}

void  SqlConnPool::PrintDebugAllInfo()
{
	uint32_t	AllTime = 0;
	uint32_t	AllCount = 0;
	for(list<CSqlClientHandle*>::iterator it = m_connList.begin(); it != m_connList.end(); ++it)
	{
		CSqlClientHandle *tempHandle = (CSqlClientHandle*)*it;

		if(tempHandle)
		{
			g_logger.debug("线程：%u，时间：%d，次数：%d,结束时间：%d",tempHandle->m_pSqlThread->CurrentThreadIdx(),tempHandle->m_Time,tempHandle->n_Count,tempHandle->m_NowTime);
			AllTime += tempHandle->m_Time;
			AllCount += tempHandle->n_Count;
		}
	}
	for (list<CSqlClientHandle*>::iterator it = m_bakList.begin(); it != m_bakList.end(); ++it)
	{
		CSqlClientHandle *tempHandle = (CSqlClientHandle*)*it;

		if(tempHandle)
		{
			g_logger.debug("线程：%u，时间：%d，次数：%d,结束时间：%d",tempHandle->m_pSqlThread->CurrentThreadIdx(),tempHandle->m_Time,tempHandle->n_Count,tempHandle->m_NowTime);
			AllTime += tempHandle->m_Time;
			AllCount += tempHandle->n_Count;
		}
	}
	g_logger.debug("finish :(Re:%d,Is:%d)(最大使用：%d)",m_sqlCacheReceive.size(),m_sqlCacheIssue.size(),m_MaxCache);
	if (AllCount > 0)
	{
		g_logger.debug("%d条，平均时间：%d",AllCount,AllTime/AllCount);
	}
}
int SqlConnPool::execSqlAsy( const char * sql,uint32_t dwHashCode)
{
	AILOCKT(m_sqlCacheReceive);
	if (sql)
	{
		stSqlOperation data;
		data.dwHashCode = dwHashCode;
		strcpy_q(data.szSqlStr,SQL_LEN-1,sql);
		m_sqlCacheReceive.push_back(data);
		SafeIncrement(m_nSqlCountPerSapce);
		return data.dwHashCode;
	}
	return 0;
}
int SqlConnPool::execSelectSqlAsy( const char *sql,const dbCol *column,uint32_t dwHashCode)
{
	AILOCKT(m_sqlCacheReceive);
	if (sql && column)
	{
		stSqlOperation data;
		data.dwHashCode = dwHashCode;
		data.col = (dbCol*)column;
		strcpy_q(data.szSqlStr,SQL_LEN-1,sql);
		m_sqlCacheReceive.push_back(data);
		SafeIncrement(m_nSqlCountPerSapce);
		return data.dwHashCode;
	}
	return 0;
}

unsigned int SqlConnPool::SqlCacheCopyThread()
{
	while (true)
	{
		if (m_sqlCacheReceive.size() > 0 && !m_sqlCacheIssue.IsFull())
		{

 				INFOLOCK(m_sqlCacheIssue);//分发cache

 				INFOLOCK(m_sqlCacheReceive);//主线程接受cache

// 				g_logger.debug("pre copy:([%d]Re:%d,Is:%d)",::GetTickCount(),m_sqlCacheReceive.size(),m_sqlCacheIssue.size());
				m_MaxCache = max(m_MaxCache,m_sqlCacheReceive.size());
				m_sqlCacheIssue.merge(m_sqlCacheReceive);
				m_MaxCache = max(m_MaxCache,m_sqlCacheIssue.size());

// 				g_logger.debug("aft copy:([%d]Re:%d,Is:%d)",::GetTickCount(),m_sqlCacheReceive.size(),m_sqlCacheIssue.size());

 				UNINFOLOCK(m_sqlCacheReceive);

/*
				if (m_sqlCacheIssue.size() > 0)//有任务
				{
					AILOCKT(mlock);

					int nMoreCont = m_sqlCacheIssue.size() - (m_connList.size()*20);//多出的数量
					if (nMoreCont > 0)
					{
						AddConnectionFromBak(nMoreCont/20);//开启几个
					}
					else if (nMoreCont < 0)
					{
						DelConnectionFromActive(abs(nMoreCont)/20);//关闭几个
					}



					for (list<CSqlClientHandle*>::iterator it = m_connList.begin(); it != m_connList.end(); ++it)// 唤醒所有活跃线程
					{
						CSqlBase* Conn = (CSqlBase*)(*it);
						if(Conn && Conn->m_pSqlThread)
						{
							INFOLOCK(Conn->m_ProcessCondLock.getCIntLock());
							Conn->m_ProcessCondLock.setEvent();
							UNINFOLOCK(Conn->m_ProcessCondLock.getCIntLock());
						}
					}
				}*/

				UNINFOLOCK(m_sqlCacheIssue);
		}
		if(m_sqlCacheReceive.size() > 0)
		{
			Sleep_Q(1);
		}
		else
		{
			Sleep_Q(5);
		}
	}
	return 1;
}
#define  PERSECOND_THREAD_SQL_NUM	200	//一个线程每秒运行的sql条数
#define  CHECKTHREAD_SPACE_TIME		5	//检测运行间隔（秒）
#define  DAEMON_RECONNECT_TIME		30	//检测程重链间隔（秒）
unsigned int SqlConnPool::SqlConnectCheckThread( )
{
	while (true)
	{
		Sleep_Q(CHECKTHREAD_SPACE_TIME*1000);		//先休眠

		//只有重连 没有换列表
		AILOCKT(mlock);

		time_t dwNowTime = time(NULL);
		//重连检查
		if (dwNowTime > m_LastReConnectTime +DAEMON_RECONNECT_TIME)
		{
			list<CSqlClientHandle*>::iterator it,itNext;
			for(it = m_bakList.begin(),itNext = it; it != m_bakList.end(); it = itNext)//重新链接未连接的对象
			{
				++itNext;
				CSqlBase* reConn = (CSqlBase*)(*it);
				if (reConn->bCoonect() == false)//需要重新连接
				{
					reConn->ReOpenDB();
				}
				else
				{
					reConn->ReConnect();	//重连
				}
			}

			for(it = m_connList.begin(),itNext = it; it != m_connList.end(); it = itNext)//检测活跃链接重连
			{
				++itNext;
				CSqlBase* reConn = (CSqlBase*)(*it);
				if (reConn->ReConnect() == false)/*重连失败*/
				{
					m_connList.erase(it);							//从激活列表中移除
					m_bakList.push_back(reConn);					//添加到休眠列表
					g_logger.error("重连失败,加入休眠列表");
				}
			}
			m_LastReConnectTime = time(NULL);
			g_logger.debug("进入重连检测，当前时间：%d,活跃线程数：%d,休眠线程数：%d",m_LastReConnectTime,m_connList.size(),m_bakList.size());
		}

		//线程调度
		{
			int nSqlCountPerSapce = SafeGet(m_nSqlCountPerSapce);
			int nTreadCount = nSqlCountPerSapce/(PERSECOND_THREAD_SQL_NUM*CHECKTHREAD_SPACE_TIME);
			if (nSqlCountPerSapce%(PERSECOND_THREAD_SQL_NUM*CHECKTHREAD_SPACE_TIME) != 0)
			{
				nTreadCount++;
			}
			if ((int)m_connList.size() < nTreadCount) //不够
			{
				if (m_bakList.size() > 0)
				{
					int nOpenCount = nTreadCount - (int)m_connList.size();
					AddConnectionFromBak(nOpenCount);//开启几个
				}
			}
			else if ((int)m_connList.size() > nTreadCount && m_UrlInfo.getmaxhandlebuf() >= 2)//过剩
			{
				int nHalfCount = m_UrlInfo.getmaxhandlebuf()/2;
				if ((int)m_connList.size() > nHalfCount)//留下一半
				{
					int nCloseCount = min((int)m_connList.size() - nTreadCount,(int)m_connList.size() - nHalfCount);
					DelConnectionFromActive(nCloseCount);
				}
			}
			SafeSet(m_nSqlCountPerSapce,0);
		}
	}
	return 1;
}

bool SqlConnPool::AddSqlRestult( uint32_t dwHashCode,int nEffectCount,dbCol* col,MYSQL_RES* MyResult )
{
	AILOCKT(ResultMap);
	CSyncMap<uint32_t,stSqlResult>::iterator it = ResultMap.find(dwHashCode);
	if (it == ResultMap.end())
	{
		stSqlResult resultdate;
		resultdate.dwHashCode = dwHashCode;
		resultdate.nEffectLine = nEffectCount;
		resultdate.col = col;
		resultdate.MyResult = MyResult;
		ResultMap.insert(CSyncMap<uint32_t,stSqlResult>::value_type(resultdate.dwHashCode,resultdate));
		return true;
	}
	else
	{
		g_logger.debug("尝试重复添加结果集");
		return false;
	}
}
int SqlConnPool::GetSqlRestult( uint32_t dwHashCode,dbCol* col, unsigned char* pInData )
{
	AILOCKT(ResultMap);
	CSyncMap<uint32_t,stSqlResult>::iterator it = ResultMap.find(dwHashCode);
	if (it != ResultMap.end())
	{
		if (it->second.col == col) //证明取的是相同的数据
		{
			MYSQL_RES* result = it->second.MyResult;
			if (result)
			{
				MYSQL_ROW row;

				if (pInData)
				{
					int nrowcount=0;
					int num_fields = mysql_num_fields(result);//获得列数
					int nStSize =  CSqlBase::getColInfoMaxStLen(col);//一行长度
					if (nStSize<0)
					{
						return -1;
					}

					unsigned char* pStartData= pInData;
					dbCol** dbArrary=new dbCol*[num_fields];
					while ((row = mysql_fetch_row(result)))//行
					{
						unsigned char *pin=pStartData;

						for(int i = 0; i < num_fields; i++)//列
						{
							if (i == 0)
							{
								int narry=0;
								MYSQL_FIELD* field = mysql_fetch_field(result);
								while(field)
								{
									dbArrary[narry]= CSqlBase::findDbCol(col,field->name,narry+1);
									narry++;
									field = mysql_fetch_field(result);
								}
							}
							dbCol*temp= dbArrary[i];
							if (temp)
							{
								if (temp->data_addr)//static
								{
									pin=temp->data_addr;
								}
								else
								{
									pin=pStartData+temp->data_offset;
								}

								CSqlBase::putUserData(temp->type,row[i],( char*)pin,temp->type_size);
							}
						}
						nrowcount++;
						pStartData+=nStSize; //下一行开始 偏移

					}
					mysql_free_result(result);
					delete[]dbArrary;

					ResultMap.erase(it);
					return nrowcount;
				}
			}
			else
			{
				ResultMap.erase(it);
				g_logger.error("sql结果集异常");
			}
		}
		else
		{
			g_logger.error("尝试获取不同类型的结果");
		}
	}
	else
	{
		g_logger.error("尝试获取不存在的结果集");
	}
	return -1;
}

int SqlConnPool::GetEffectCount(uint32_t dwHashCode,bool boDel)
{
	AILOCKT(ResultMap);
	CSyncMap<uint32_t,stSqlResult>::iterator it = ResultMap.find(dwHashCode);
	if (it != ResultMap.end())
	{
		int nLine = it->second.nEffectLine;
		if (boDel)//删除
		{
			ResultMap.erase(it);
		}
		return nLine;
	}
	return 0;
}

bool SqlConnPoolManage::addPool( stUrlInfo& ui )
{
	ConnPoolList::iterator it = m_PoolList.find(ui.hashcode);
	if (it == m_PoolList.end())
	{
		SqlConnPool* mypool = new SqlConnPool(ui);
		m_PoolList.insert(ConnPoolList::value_type(ui.hashcode,mypool));
		mypool->InitConnection();
		return true;
	}
	else
	{
		g_logger.error("发现ID重复的数据库连接,增加数据库连接失败！");
		return false;
	}
	return false;
}

SqlConnPool* SqlConnPoolManage::GetPool( unsigned int nhashcode )
{
	ConnPoolList::iterator it = m_PoolList.find(nhashcode);
	if (it != m_PoolList.end())
	{
		return (SqlConnPool*)it->second;
	}
	return NULL;
}

SqlConnPoolManage::SqlConnPoolManage()
{
	m_PoolList.clear();
}
void SqlConnPoolManage::PrintDebugAllInfo(unsigned int nhashcode)
{
	ConnPoolList::iterator it = m_PoolList.find(nhashcode);
	if (it != m_PoolList.end())
	{
		SqlConnPool* pPool = (SqlConnPool*)it->second;
		if (pPool)
		{
			pPool->PrintDebugAllInfo();
		}
	}
}
