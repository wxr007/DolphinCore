#include "sql/sqltool.h"
CSqlPool::CSqlPool()
{
    m_mpSqlTool.clear();
}

CSqlPool::~CSqlPool()
{
    m_mpSqlTool.clear();
}

bool CSqlPool::AddSqlBase(CSqlBase * pSql,int nIndex)
{
    if(pSql&&nIndex>=0)
    {
        MpSqlTool::iterator it;
        it = m_mpSqlTool.find(nIndex);
        if(it == m_mpSqlTool.end())
        {
            m_mpSqlTool.insert(MpSqlTool::value_type(nIndex, pSql));
            return true;
        }
    }
    return false;
}

void CSqlPool::DelSqlBase(int nIndex)
{
    if(nIndex >= 0)
    {
        MpSqlTool::iterator it;
        it = m_mpSqlTool.find(nIndex);
        if(it != m_mpSqlTool.end())
        {
            m_mpSqlTool.erase(it);
        }
    }
}

CSqlBase * CSqlPool::FindByIndex(int nIndex)
{
    if(nIndex >= 0)
    {
        MpSqlTool::iterator it;
        it = m_mpSqlTool.find(nIndex);
        if(it != m_mpSqlTool.end())
        {
			CSqlBase* csql =it->second;
			if (csql&&csql->bDead())
			{
				csql->ReOpenDB();
			}
            return csql;
        }
    }
    return NULL;
}

//stAutoSqlClient

stAutoSqlClient::stAutoSqlClient(DBConnPool* pool, unsigned int nhashcode)
{
	m_Sql = NULL;
	m_pool = NULL;

	if(pool)
	{
		m_pool = pool;
		m_Sql = m_pool->getSqlClient(nhashcode);
	}
}

stAutoSqlClient::stAutoSqlClient(DBConnPool& pool, unsigned int nhashcode)
{
	m_pool = &pool;
	m_Sql = m_pool->getSqlClient(nhashcode);
}

stAutoSqlClient::~stAutoSqlClient()
{
	if(m_pool && m_Sql)
	{
		m_pool->putSqlClient(m_Sql);
	}

	m_Sql = NULL;
	m_pool = NULL;
}




CSqlClientHandle* DBConnPool::getHandleByHashcode(unsigned int hashcode)
{
	FUNCTION_BEGIN;

	handlesPool::iterator it;
	AILOCKT(mlock);
	it=handles.find(hashcode);
	if (it==handles.end())
	{
		return NULL;
	}
	
	CSqlBase* csql=it->second;
	if (csql&&csql->bDead())
	{
		csql->ReOpenDB();
	}
	return csql;
}


