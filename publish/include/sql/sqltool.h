#ifndef _SQLTOOL_H__
#define _SQLTOOL_H__
#include "sql/sqlHelper.h"
#include "logging/logging/zLogger.h"

class CSqlPool
{
public:
    typedef std::map< int, CSqlBase*>  MpSqlTool;
    MpSqlTool m_mpSqlTool;

    CSqlBase * FindByIndex(int nIndex);                 //通过Index找到sqlbase指针 需要判断指针是否为空
    bool       AddSqlBase(CSqlBase * pSql,int nIndex);
    void       DelSqlBase(int nIndex);
    CSqlPool();
    ~CSqlPool();
};


typedef unsigned int connHandleID;

typedef hash_multimap<unsigned int, CSqlClientHandle *> handlesPool;
typedef hash_map<unsigned int, stUrlInfo> urlsPool;
typedef hash_map<connHandleID, CSqlClientHandle *> handlesIDMap;


class DBConnPool;

//只是一个简单的获得sql类 方便而已
struct stAutoSqlClient
{
private:
	CSqlClientHandle* m_Sql;
	DBConnPool* m_pool;
public:
	stAutoSqlClient(unsigned int nhashcode)
	{
		m_Sql = NULL;
		m_pool = NULL;
	}
	stAutoSqlClient(DBConnPool* pool, unsigned int nhashcode);
	stAutoSqlClient(DBConnPool& pool, unsigned int nhashcode);
	virtual ~stAutoSqlClient();

	__inline CSqlClientHandle* client()
	{
		return m_Sql;
	}
};


#define GETAUTOSQL(sqlrettype,sql,pool,nhashcode)	stAutoSqlClient ac##sql(pool,nhashcode);sqlrettype sql=dynamic_cast<sqlrettype>(ac##sql.client());

//数据库连接池 管理多个 sql 类
class DBConnPool
{
private:
	CIntLock mlock;
	handlesPool handles;//记录sql类指针 map
	urlsPool urls;      //记录 sql url map
	handlesIDMap idmaps;//没什么用
public:
	DBConnPool()
	{
	}

	~DBConnPool()
	{
		clear();
	}

	bool putURL(unsigned int hashcode, stUrlInfo::eSqlType type, const char *host,
	            unsigned int port, const char *dbname, const char *user,
	            const char *pass, bool supportTransactions, uint8_t maxhandle)
	{
		FUNCTION_BEGIN;
		stUrlInfo::stSqlTypeInfo* typeinfo = stUrlInfo::gettypeinfo(type);

		if(typeinfo)
		{
			char buffer[1024] = {0};
			sprintf_q(buffer, sizeof(buffer), "%s%s:%s@%s:%u/%s", typeinfo->head, user, pass, host, port, dbname);
			stUrlInfo ui(hashcode, std::string(buffer), supportTransactions, maxhandle);
			return putURL(&ui);
		}

		return false;
	}

	bool putURL(stUrlInfo* ui)
	{
		if(ui)
		{
			CSqlClientHandle* psqlc = getSqlClient(ui->hashcode);

			if(psqlc != NULL)
			{
				putSqlClient(psqlc);

				if(ui->url != psqlc->geturl().url)
				{
					g_logger.error("发现ID重复的数据库连接,增加数据库连接失败！");
					return false;
				}

				return true;
			}

			CSqlClientHandle *handle = ui->newsqlclienthandle();

			if(handle == NULL)
				return false;

			if(handle->initHandle())
			{
				INFOLOCK(mlock);
				handles.insert(handlesPool::value_type(ui->hashcode, handle));
				urls.insert(urlsPool::value_type(ui->hashcode, ui));
				idmaps.insert(handlesIDMap::value_type(handle->getID(), handle));
				UNINFOLOCK(mlock);
				return true;
			}
			else
			{
				SAFE_DELETE(handle);
				return false;
			}
		}

		return false;
	}

	bool putURL(unsigned int hashcode, const char *url, bool supportTransactions, uint8_t maxhandle)
	{
		FUNCTION_BEGIN;
		stUrlInfo ui(hashcode, url, supportTransactions, maxhandle);
		return putURL(&ui);
	}

	__inline CSqlClientHandle* getSqlClient(unsigned int hashcode)
	{
		return getHandleByHashcode(hashcode);
	}

	void putSqlClient(CSqlClientHandle* handle)
	{
		FUNCTION_BEGIN;

		if(handle != NULL)
		{
			handle->unsetHandle();
		}
	}

	CSqlClientHandle* getHandleByID(connHandleID handleID);

	void clear()
	{
		INFOLOCK(mlock);

		if(!handles.empty())
		{
			for(handlesPool::iterator it = handles.begin(); it != handles.end(); ++it)
			{
				CSqlClientHandle *tempHandle = (*it).second;

				if(tempHandle)
				{
					tempHandle->finalHandle();
				};

				SAFE_DELETE(tempHandle);
			}
		}

		handles.clear();
		urls.clear();
		idmaps.clear();
		UNINFOLOCK(mlock);
	}
protected:
	CSqlClientHandle* getHandleByHashcode(unsigned int hashcode);
};



#endif//_SQLTOOL_H__
