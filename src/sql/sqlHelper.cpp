
#ifndef MYSQL_CPP
#define MYSQL_CPP

#include<stdio.h>
#include <time.h>
#include "platform/platform.h"
#include "sql/sqlHelper.h"
#include "sql/connPool.h"
#include "logging/logging/zLogger.h"
#include "serialize/easyStrParse.h"

int64_t CSqlBase::HandleID_generator = 0;

struct  mysqlstEscape
{
	const char* src;
	int nsrclen;
	const char* dest;
	int ndestlen;
};

const mysqlstEscape mysql_stc_escape[] =
{
	{"\0", 1, "CHAR(0)", 7},
	{"\'", 1, "CHAR(39)", 8},
};

//mysql 数据库sqlbase实现

CSqlBase::~CSqlBase()
{

	closeDb();
// 	if (m_pSqlThread && !m_pSqlThread->IsTerminated())
// 	{
// 		m_pSqlThread->Terminate();
// 	}
}
#define  _SQL_TickTime_		5		//时间
#define  _SQL_PreCount_		20		//每次操作条数


CSqlBase::CSqlBase():id((unsigned int )(HandleID_generator + 1)),m_sqllist(_SQL_PreCount_)
{
	AILOCKT(m_sqlLock);
	m_naffectedrows=false;
	m_bCooncet=false;
	m_bsaveonce=false;
	m_qinid=(unsigned int)-1;
	m_bocangetqinid= false;
	m_RunTickTime = 0;
	m_Time = 0;
	n_Count = 0;
	m_NowTime = 0;
	m_MyPool = NULL;
	m_Active = false;
	m_mysql = NULL;
}


CSqlBase::CSqlBase(stUrlInfo* ui):id((unsigned int )(HandleID_generator + 1)),url(ui),m_sqllist(50)
{

	if (ui)
	{
		AILOCKT(m_sqlLock);
		m_naffectedrows=false;
		m_bCooncet=false;
		m_bsaveonce=false;
		m_qinid=(unsigned int)-1;
		m_bocangetqinid= false;
		char szbuf[256]= {0};
		int nport=ui->getport();
		m_Time = 0;
		n_Count = 0;
		m_NowTime = 0;
		m_MyPool = NULL;
		m_Active = true;
		if(nport==0)
		{
			nport=3306;//sql 默认端口
		}
		m_mysql = NULL;
		sprintf_q(szbuf,256,"%s:%d",ui->gethost(),nport);
		m_RunTickTime = 0;
        m_pSqlThread = new QThread(QCOOL::bind(&CSqlBase::SqlProcessThread,this),"sql");
		bool boDB = openDB(szbuf,ui->getuser(),ui->getpasswd(),ui->getdbName());
		
		if (!boDB)
		{
			ReDead();
		}
		
	}
}
void CSqlBase::ReDead()
{
	bool boDB = true;
	do 
	{
		closeDb();
		bool boDead = openDB(m_hostip,m_username,m_passwd,m_dbName);

		if (!boDead)
		{
			g_logger.debug("连接失败5秒后重新连接%s",m_hostip);
			Sleep_Q(5*1000);
		}
		else
		{
			boDB = false;
			g_logger.debug("连接成功%s",m_hostip);
		}
	} while (boDB);
}
bool CSqlBase::closeDb()
{
	AILOCKT(m_sqlLock);
	if (m_mysql)
	{
		mysql_close(m_mysql);
	}
	return true;
}

bool CSqlBase::ReOpenDB(const char* charset/* =SQL_UTF8 */)
{

	AILOCKT(m_sqlLock);
	closeDb();
	return openDB(m_hostip,m_username,m_passwd,m_dbName);
}

bool CSqlBase::ReConnect()
{
	if (m_bCooncet)
	{
		AILOCKT(m_sqlLock);
		if (mysql_ping(m_mysql) !=0 )	//链接断开了
		{
			if(mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8") != 0)
			{
				g_logger.error("Error1 %u:%s,%s\n", mysql_errno(m_mysql),m_hostip, mysql_error(m_mysql));
				m_bCooncet = false;
			}

			char value = 1;
			if (mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &value) != 0)
			{
				g_logger.error("Error2 %u:%s, %s\n", mysql_errno(m_mysql),m_hostip, mysql_error(m_mysql));
				m_bCooncet =  false;
			}

			CEasyStrParse parse;
			char szCh[] = {":"};
			parse.SetParseStrEx(m_hostip, szCh);
			if (mysql_real_connect(m_mysql,parse[0],m_username,m_passwd, m_dbName, atoi(parse[1]), NULL, 0) == NULL)//重新链接
			{
				g_logger.error("Error3 %u:%s, %s\n", mysql_errno(m_mysql), m_hostip,mysql_error(m_mysql));
				m_bCooncet = false;
			}

			if(mysql_query(m_mysql, "set names 'utf8'")!=0)
			{
				printf("set names utf8 error(%s)\n",mysql_error(m_mysql));
				m_bCooncet = false;
			}
		}
	}
	if (m_bCooncet == false)
	{
		m_Active = false;
	}
	return m_bCooncet;
}

bool CSqlBase::openDB(const char* server, const char* user, const char* password,const char* dbname, const char* charset/* =SQL_UTF8 */)
{
	m_bCooncet=false;
	if(server==NULL || user==NULL||password==NULL||dbname==NULL)
	{
		g_logger.error("Server-address or db-name you supplied is null, that is not allowed.");
		return false;
	}

	if (m_bsaveonce==false)
	{
		m_bsaveonce=true;
		strcpy_q(m_hostip,sizeof(m_hostip),server);
		strcpy_q(m_username,sizeof(m_username),user);
		strcpy_q(m_passwd,sizeof(m_passwd),password);
		strcpy_q(m_dbName,sizeof(m_dbName),dbname);
	}

	m_mysql = mysql_init(NULL);
	if (m_mysql == NULL) 
	{
		g_logger.error("Error4 %u:%s, %s\n", mysql_errno(m_mysql),server, mysql_error(m_mysql));
		return false;
	}
	if(mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8") != 0)
	{
	    g_logger.error("Error5 %u:%s, %s\n", mysql_errno(m_mysql),server, mysql_error(m_mysql));
		return false;
	}
	char value = 1;
	if (mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &value) != 0)
	{
		g_logger.error("Error6 %u:%s, %s\n", mysql_errno(m_mysql),server, mysql_error(m_mysql));
		return false;
	}

	CEasyStrParse parse;
	char szCh[] = {":"};
    parse.SetParseStrEx(server, szCh);
	if (mysql_real_connect(m_mysql, parse[0], user, password, dbname, atoi(parse[1]), NULL, 0) == NULL)
	{
			g_logger.error("Error7 %u:%s, %s\n", mysql_errno(m_mysql),server, mysql_error(m_mysql));
			return false;
			
	}
	if(mysql_query(m_mysql, "set names 'utf8'")!=0)
	{
		printf("set names utf8 error(%s)\n",mysql_error(m_mysql));
		return false;
	}
	m_bCooncet=true;
	return true;
}

bool    CSqlBase::bDead()
{
	AILOCKT(m_sqlLock);
	return mysql_ping(m_mysql)!=0;
}

bool CSqlBase::bCoonect()
{
	return m_bCooncet;
}

int64_t CSqlBase::execSql(const char * sql,size_t n)
{
		AILOCKT(m_sqlLock);
		m_bocangetqinid = false;
		m_naffectedrows=extInsertUpdate(sql);
		if (m_naffectedrows == -1)
		{
			g_logger.error("执行SQL语句出错%s，%s", sql, __FUNC_LINE__);
		}
		
		return m_naffectedrows;
}

int64_t CSqlBase::getaffectedrows()
{
	AILOCKT(m_sqlLock);
	return m_naffectedrows;
}

bool CSqlBase::bExecuteSql(const char * sql)
{   	
	if (sql==NULL||m_mysql==NULL)
	{
		return false;
	}
	if(mysql_query(m_mysql, "set names 'utf8'")!=0)
	{
	    printf("set names utf8 error(%s)\n",mysql_error(m_mysql));
		return false;
	}
	if( mysql_query(m_mysql,sql)!= 0)
	{
		g_logger.error("write sql error(%s)(%s)",mysql_error(m_mysql),sql);
		return false;
	}
	return true;
}

uint64_t CSqlBase::extInsertUpdate(const char*sql)
{
	if (sql==NULL||m_mysql==NULL)
	{
		m_bexesqlResult=false;
		return -1;
	}
	if(mysql_query(m_mysql, "set names 'utf8'")!=0)
	{
	    printf("set names utf8 error(%s)\n",mysql_error(m_mysql));
		return -1;
	}
	if( mysql_query(m_mysql,sql)!= 0)
	{
		printf("write sql error(%s)\n",mysql_error(m_mysql));
		m_bexesqlResult=false;
		return -1;
	}

	uint64_t naffcetrows=mysql_affected_rows(m_mysql);//返回受影响行数
	return naffcetrows;
}

bool CSqlBase::writeData(const char * sql)
{
	AILOCKT(m_sqlLock);
	return bExecuteSql(sql);
}

int64_t CSqlBase::getCount(const char* tbName,const char* sql)
{

	char szSqlBuf[MAX_SQL_SELECTLEN]= {0};
	char szSql[MAX_SQL_SELECTLEN]= {0};
	sprintf_q(szSqlBuf,MAX_SQL_SELECTLEN,"SELECT  COUNT(*)  FROM %s ",tbName);
	strcpy_q(szSql,MAX_SQL_SELECTLEN,szSqlBuf);
	if (sql)
	{
		sprintf_q(szSql,MAX_SQL_SELECTLEN,"%s where %s",szSqlBuf,sql);
	}

	AILOCKT(m_sqlLock);

	if (!bExecuteSql(szSql))
	{
		return -1;
	}

	MYSQL_RES *result;
	MYSQL_ROW rowdata;
	int rowCount=0;
	result = mysql_store_result(m_mysql);
	if (result)
	{
		rowdata = mysql_fetch_row(result);
		if (rowdata)
			rowCount = atoi(rowdata[0]);
		else rowCount = -7;   // data error
	}
	mysql_free_result(result);
	return rowCount;
}

bool CSqlBase::escapebin2str(const char *src, size_t srcsize, char *dest)
{
	if(src == NULL || dest == NULL)
	{
		return false;
	}

	dest[0] = 0;

	if(srcsize == 0)
	{
		return true;
	}

	size_t nmaxhex = safe_min<size_t>(safe_max<size_t>((srcsize / 16), 1), 50);
	size_t i = 0;
	size_t nhex = 0;

	while(i < srcsize)
	{
		if((uint8_t)src[i] >= 0x80)
		{
			i++;

			if(!(i < (srcsize - 1)))
			{
				nhex++;
			}
		}
		else
		{
			for(unsigned int ii = 0; ii < count_of(mysql_stc_escape); ii++)
			{
				if(strncmp(&src[i], mysql_stc_escape[ii].src, mysql_stc_escape[ii].nsrclen) == 0)
				{
					i = i + (mysql_stc_escape[ii].nsrclen - 1);
					nhex++;
					break;
				}
			}
		}

		if(nhex > nmaxhex)
		{
			break;
		}

		i++;
	}

	if(nhex > nmaxhex)
	{
		char *pdata = dest;
		strcat(dest, "0x");
		dest += 2;
		dest[0] = 0;

		for(i = 0; i < sizeof(src); i++)
		{
			sprintf_q(dest, 3, "%.2x", (uint8_t)src[i]);
			dest += 2;
			dest[0] = 0;
		}

		size_t nlen = strlen(pdata);
		char *strDes = new char[nlen + 1];
		strDes=strcpy(strDes,pdata);
		dest = pdata;
		sprintf_q(dest, sizeof(CONVERT_BIN_BEGIN_MYSQL) + 16, CONVERT_BIN_BEGIN_MYSQL,strDes,srcsize);
		dest += strlen(dest);
		dest[0] = 0;
		SAFE_DELETE_VEC(strDes);
		return false;
	}
	else
	{
		char *pdata = dest;
		dest[0] = '\'';
		dest++;
		dest[0] = 0;
		escapeStr(src, dest, srcsize);
		strcat(dest, "\'");
		size_t nlen = strlen(pdata);
		char *strDes = new char[nlen + 1];
		strDes=strcpy(strDes,pdata);
		dest = pdata;
		sprintf_q(dest, sizeof(CONVERT_BIN_BEGIN_MYSQL) + 16, CONVERT_BIN_BEGIN_MYSQL,strDes, srcsize);
		dest += strlen(dest);
		dest[0] = 0;
		SAFE_DELETE_VEC(strDes);
		return true;
	}
}


char * CSqlBase::escapeStr(const char *src, char *dest, size_t srcsize)
{
	FUNCTION_BEGIN;
	if(srcsize <= 0)
	{
		srcsize = strlen(src);
	}

	if(dest)
	{
		dest[0] = 0;
	}

	if(src == NULL || dest == NULL || srcsize == 0) return dest;

	dest[0] = 0;
	char* bakdest = dest;
	bool bolastescape = false;
	unsigned int i = 0;
	while(i < srcsize)
	{
		bool boescape = false;

		if((uint8_t)src[i] >= 0x80)
		{
			char sztext[128] = {0};
			int nsrclen = 3;

			if(i < (srcsize - 1))
			{
				{
					if(bolastescape)
					{
						strcat(dest, "+\'");
						dest += 2;
						dest[0] = 0;
					}

					dest[0] = src[i];
					dest[1] = src[i+1];
					dest[2] = src[i+2];
					dest += nsrclen;
					i += nsrclen;
					dest[0] = 0;
					bolastescape = false;
					continue;
				}
			}
			else
			{
				sprintf_q(sztext, sizeof(sztext), "CHAR(%u)\0", (uint8_t)src[i]);
				nsrclen = 1;
			}

			if(!bolastescape)
			{
				strcat(dest, "\'+");
				dest += 2;
				dest[0] = 0;
			}
			else
			{
				dest[0] = '+';
				dest++;
				dest[0] = 0;
			}

			strcat(dest, sztext);
			dest += strlen(sztext);
			dest[0] = 0;

			if((i + nsrclen) >= srcsize)
			{
				strcat(dest, "+\'");
				dest += 2;
				dest[0] = 0;
			}

			boescape = true;
			i += nsrclen;
		}
		else
		{
			for(unsigned int ii = 0; ii < count_of(mysql_stc_escape); ii++)
			{
				if(strncmp(&src[i], mysql_stc_escape[ii].src, mysql_stc_escape[ii].nsrclen) == 0)
				{
					if(!bolastescape)
					{
						strcat(dest, "\'+");
						dest += 2;
						dest[0] = 0;
					}
					else
					{
						dest[0] = '+';
						dest++;
						dest[0] = 0;
					}

					strcat(dest, mysql_stc_escape[ii].dest);
					dest += mysql_stc_escape[ii].ndestlen;
					dest[0] = 0;

					if((i + mysql_stc_escape[ii].nsrclen) >= srcsize)
					{
						strcat(dest, "+\'");
						dest += 2;
						dest[0] = 0;
					}

					boescape = true;
					i += mysql_stc_escape[ii].nsrclen;
					break;
				}
			}
		}

		if(!boescape)
		{
			if(bolastescape)
			{
				strcat(dest, "+\'");
				dest += 2;
				dest[0] = 0;
			}

			dest[0] = src[i];
			dest++;
			dest[0] = 0;
			bolastescape = false;
			i++;
		}
		else
		{
			bolastescape = true;
		}
	}

	return bakdest;
}

int64_t CSqlBase::sprintf_exec(const char * pattern, ...)
{
	FUNCTION_BEGIN;
	char szTemp[1024*8] = {0};
	va_list ap;
	va_start(ap, pattern);
	vsprintf_q(szTemp, (sizeof(szTemp)) - 1, pattern, ap);
	va_end(ap);
	return execSql(szTemp, strlen(szTemp));
}

int64_t CSqlBase::execInsert(const char *tableName,const dbCol *column,const unsigned char *data,
						 const char* noexists_where,const char* noexists_table,void* table)
{
	FUNCTION_BEGIN;
	const dbCol *temp;

	if(tableName == NULL || data == NULL || column == NULL )
	{
		//g_logger.error("null pointer error. ---- %s", __FUNC_LINE__);
		return (unsigned int) - 1;
	}

	std::ostringstream strSql;
	strSql << "INSERT INTO ";
	strSql << tableName;
	strSql << " ( ";
	temp = column;
	bool first = true;

	while(temp->name)
	{
		if(temp->name[0] != 0)
		{
			if(first)
				first = false;
			else
				strSql << ", ";

			strSql << temp->name;
		}

		temp++;
	}

	if(!noexists_where)
	{
		strSql << ") VALUES( ";
	}
	else
	{
		strSql << ") SELECT ";
	}

	first = true;
	temp = column;

	while(temp->name)
	{
		if(temp->name[0] != 0 && temp->canWrite())
		{
			if(first)
				first = false;
			else
				strSql << ", ";

			if(userdata2sql(temp->type, strSql, temp->data_addr == NULL ? data : temp->data_addr, temp->type_size, temp->data_offset) < 0)
			{
				return (unsigned int) - 1;
			}
		}

		temp++;
	}

	if(!noexists_where)
	{
		strSql << ")";
	}
	else
	{
		strSql << " where not exists (select * from ";

		if(noexists_table)
		{
			strSql << noexists_table;
		}
		else
		{
			strSql << tableName;
		}

		strSql << " where ";
		strSql << noexists_where << ")";
	}

	return execSql(strSql.str().c_str(), strSql.str().length());
}

int64_t CSqlBase::execUpdate(const char *tableName, const dbCol *column,
						 const unsigned char *data, const char *where, void* table)
{
	FUNCTION_BEGIN;
	std::ostringstream out_sql;
	const dbCol *temp;

	if(tableName == NULL || column == NULL || data == NULL )
	{
		//		g_logger.error("null pointer error. ---- %s", __FUNC_LINE__);
		return (unsigned int) - 1;
	}

	out_sql << "UPDATE " << tableName << " SET ";
	temp = column;
	bool first = true;

	while(temp->name)//以dbcol 为核心进行偏移 拼凑sql
	{
		if(temp->name[0] != 0 && temp->canWrite())
		{
			if(first)
				first = false;
			else
				out_sql << ", ";

			out_sql << temp->name << "=";

			if(userdata2sql(temp->type, out_sql, temp->data_addr == NULL ? data : temp->data_addr, temp->type_size, temp->data_offset) < 0)
			{
				return (unsigned int) - 1;
			}
		}

		temp++;
	}

	if(where != NULL)
	{
		out_sql << " WHERE " << where;
	}

	return execSql(out_sql.str().c_str(), out_sql.str().length());
}

int64_t CSqlBase::execSelectSqlRealWork(const char * szSql,dbCol* col, unsigned char* pInData)
{
    if(szSql==NULL||col==NULL||pInData==NULL)
    {
        return -1;
    }
    AILOCKT(m_sqlLock);

    if(!bExecuteSql(szSql))
    {
        sprintf_q(errMsg,ERR_MSG_LEN-1,"dbsqlexec error(%s)",szSql);
        return -1;
    }

	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field;

	int32_t num_fields = 0;
	int32_t i = 0;
	result = mysql_store_result(m_mysql);
	num_fields = mysql_num_fields(result);//获得列数
	int32_t nStSize=getColInfoMaxStLen(col);
	if (nStSize<0)
	{
		return 0;
	}

	unsigned char* pStartData=pInData;
	dbCol** dbArrary=new dbCol*[num_fields];
	int nrowcount=0;
	while ((row = mysql_fetch_row(result)))//行
	{
		unsigned char *pin=pStartData;


		for(i = 0; i < num_fields; i++)//列
		{
			if (i == 0)
			{
				int narry=0;
				field = mysql_fetch_field(result);
				while(field)
				{
					dbArrary[narry]=findDbCol(col,field->name,narry+1);
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

				putUserData(temp->type,row[i],( char*)pin,temp->type_size);
			}
		}
		nrowcount++;
		pStartData+=nStSize; //下一行开始 偏移

	}
	mysql_free_result(result);
	delete[]dbArrary;
   
    return nrowcount;

}

int64_t CSqlBase::execSelectSql(const char *sql , const dbCol *column,
							unsigned char* data, unsigned int maxbuflen  , unsigned int sqllen )
{
	if(data != NULL)
	{
		unsigned char* ptempdata = data;
		m_bocangetqinid = false;
 		return execSelectSqlRealWork(sql, (dbCol*)column, ptempdata);
	}
	return -1;
}

int64_t CSqlBase::execDelete(const char *tableName, const char *where)
{
	FUNCTION_BEGIN;
	if(tableName==NULL )
	{
		//        g_logger.error("null pointer error. ---- %s",__FUNC_LINE__);
		return (unsigned int)-1;
	}
	std::string strSql="DELETE FROM ";
	strSql+=tableName;
	if(where)
	{
		strSql+=" WHERE ";
		strSql+=where;
	}

	return execSql(strSql.c_str(), strSql.length());
}


int64_t CSqlBase::exexSelectMultiBinDataSql(const char *szSql ,const dbCol *col,unsigned char* pInData,unsigned int maxbuflen,unsigned int sqllen,char **ppArray )
{
	if(szSql==NULL||col==NULL||pInData==NULL)
	{
		return -1;
	}
	AILOCKT(m_sqlLock);

	if(!bExecuteSql(szSql))
	{
		sprintf_q(errMsg,ERR_MSG_LEN-1,"dbsqlexec error(%s)",szSql);
		return -1;
	}



	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field;

	int num_fields;
	int i;
	result = mysql_store_result(m_mysql);
	num_fields = mysql_num_fields(result);//获得列数
	int nStSize=getColInfoMaxStLen(col);
	if (nStSize<0)
	{
		return 0;
	}

	unsigned char* pStartData=pInData;
	dbCol** dbArrary=new dbCol*[num_fields];
	int nrowcount=0;//行数
	while ((row = mysql_fetch_row(result)))//行
	{
		unsigned char *pin=pStartData;


		for(i = 0; i < num_fields; i++)//列
		{
			if (i == 0)
			{
				int narry=0;
				field = mysql_fetch_field(result);
				while(field)
				{
					dbArrary[narry]=findDbCol((dbCol*)col,field->name,narry+1);
					narry++;
					field = mysql_fetch_field(result);
				}
			}
			dbCol*temp= dbArrary[i];
			if (temp)
			{

				if (strcmp(temp->name,"bindata")==0)
				{
					pin=(unsigned char *)ppArray[nrowcount];
				}
				else
				{
					if (temp->data_addr)//static
					{
						pin=temp->data_addr;
					}
					else
					{
						pin=pStartData+temp->data_offset;
					}
				}					

				putUserData(temp->type,row[i],( char*)pin,temp->type_size);
			}
		}
		nrowcount++;
		pStartData+=nStSize; //下一行开始 偏移

	}
	mysql_free_result(result);
	delete[]dbArrary;

	return nrowcount;
}

unsigned int CSqlBase::getColInfoMaxStLen(const dbCol* column)
{
	FUNCTION_BEGIN;
	unsigned int nStLen = 0;

	if(column == NULL) return nStLen;

	const dbCol *temp;
	temp = column;

	while (temp->name)
	{
		temp++;
	}

	if(temp->type_size > 0)//最后一位保存了实际 该结构体的 大小
	{
		nStLen = temp->type_size;
	}

	return nStLen;
}


eSD2U_RC CSqlBase::userdata2sql(int _etype, std::ostringstream& sqlout, const unsigned char * userbuf,const size_t dwuserlen, size_t offset)
{
	FUNCTION_BEGIN;

	switch(_etype)
	{
	case DB_BYTE:
		{
			sqlout << (int)((0x00ff & (*(uint8_t *)(userbuf + offset))));
			offset += sizeof(uint8_t);//uint8_t
		}
		break;
	case DB_WORD:
		{
			sqlout << (int)((*(uint16_t *)(userbuf + offset)));
			offset += sizeof(uint16_t);//uint16_t
		}
		break;
	case DB_DWORD:
		{
			sqlout << (int)((*(uint32_t *)(userbuf + offset))); //WARN：uint32_t 类型在存储的时候用int会有可能出现负数(只要数据结构定义为uint32_t读取OK)
			offset += sizeof(uint32_t);
		}
		break;
	case DB_QWORD:
		{
			sqlout << *(int64_t *)(userbuf + offset);
			offset += sizeof(int64_t);//int64_t
		}
		break;
	case DB_FLOAT:
		{
			sqlout << *(float *)(userbuf + offset);
			offset += sizeof(float);
		}
		break;
	case DB_DOUBLE:
		{
			sqlout << *(double *)(userbuf + offset);
			offset += sizeof(double);
		}
		break;
	case DB_RAWSTR:
		{
			size_t len = strlen((char *)(userbuf + offset));
			len = safe_min(len, dwuserlen);

			if(dwuserlen > 0)
			{
				ZSTACK_ALLOCA(char* , strData, len * 12 + 128);
				escapeStr((char *)(userbuf + offset), strData, len);
				sqlout << strData ;
			}
			else
			{
				sqlout << "\'\'";
			}

			offset += dwuserlen;
		}
		break;
	case DB_STR:
		{
			size_t len = strlen((char *)(userbuf + offset));
			len = safe_min(len, dwuserlen);

			if(dwuserlen > 0)
			{
				ZSTACK_ALLOCA(char* , strData, len * 12 + 128);
				escapeStr((char *)(userbuf + offset), strData, len);
				sqlout << "\'" << strData << "\'";
			}
			else
			{
				sqlout << "\'\'";
			}

			offset += dwuserlen;
		}
		break;
	case DB_DATETIME:
		{
			sqlout << "\'" << timetostr(*(uint32_t *)(userbuf + offset)) << "\'";
			offset += sizeof(uint32_t);
		}
		break;
	case DB_BIN:
		{
			if(dwuserlen > 0)
			{
				ZSTACK_ALLOCA(char* , strData, dwuserlen * 12 + 128);
				escapebin2str((char *)(userbuf + offset), dwuserlen, strData);
				sqlout << strData;
			}
			else
			{
				sqlout << "CONVERT(varbinary(0),\'\')";
			}

			offset += dwuserlen;
		}
		break;
	case DB_BIN2:
		{
			unsigned int size = *((uint32_t *)(userbuf + offset));

			if(dwuserlen != 0 && size > dwuserlen)
			{
				size += sizeof(uint32_t);
				offset += size;
				sqlout << "CONVERT(varbinary(0),\'\')";
				return eDataSqlUserRetSizeError;
			}

			size += sizeof(uint32_t);
			ZSTACK_ALLOCA(char* , strData, size * 12 + 128);
			escapebin2str((char *)(userbuf + offset), size, strData);
			sqlout << strData ;
			offset += size;
		}
		break;
	case DB_ZIP2:
	case DB_ZIP:
		{
			if(!(dwuserlen == 0 && _etype == DB_ZIP))
			{
				size_t size = 0;

				if(_etype == DB_ZIP)
				{
					size = dwuserlen;
				}
				else
				{
					unsigned int size = *((uint32_t *)(userbuf + offset));

					if(dwuserlen != 0 && size > dwuserlen)
					{
						size += sizeof(uint32_t);
						offset += size;
						sqlout << "CONVERT(varbinary(0),\'\')";
						return eDataSqlUserRetSizeError;
					}

					//size += sizeof(uint32_t);
				}

				uLongf destLen = (uLongf)size * ((120 / 100) + 1) + 32;
				ZSTACK_ALLOCA(Bytef* , destBuffer, destLen * sizeof(Bytef));
				int retcode = compress(destBuffer, &destLen, (Bytef *)(userbuf + offset), size);

				switch(retcode)
				{
				case Z_OK:
					{
						ZSTACK_ALLOCA(char* , strData, destLen * 12 + 128);
						escapebin2str((char *)destBuffer, destLen, strData);
						sqlout << strData;
					}
					break;
				case Z_MEM_ERROR:
				case Z_BUF_ERROR:
					{
						sqlout << "CONVERT(varbinary(0),\'\')";
						offset += size;
						return eDataSqlUserRetSizeError;
					}
				}

				offset += size;
			}
		}
		break;
	default:
		{
			offset += dwuserlen;
			//	g_logger.error("[ %s ] %s invalid type...", __FUNC_LINE__ , getTypeString(_etype));
			return eDataSqlUserRetTypeError;
		}
	}

	return eDataSqlUserRetOk;
}

dbCol* CSqlBase::findDbCol(dbCol* dbColArr,const char * name,int nindex)
{
	int n=nindex;
    if (dbColArr==NULL||name==NULL)
    {
        return NULL;
    }


    while(dbColArr->name)
    {
		n--;
		if (stricmp_q(dbColArr->name,"*")==0&&n==0)
		{
			return dbColArr;
		}
        else if (stricmp_q(dbColArr->name, name) == 0)//* 代表占位 放入数据的时候则根据 占位位置进行数据放入
        {
            return dbColArr;
        }
        dbColArr++;
    }

    return NULL;
}


int CSqlBase::putUserData(int _etype,const char*sqlData, char* userbuf,const unsigned int dwuserMaxlen)
{
	FUNCTION_BEGIN;


	switch(_etype)
	{
	case DB_BYTE:
		{
			if(sqlData > 0)
				*(uint8_t *)(userbuf) = (uint8_t)atoi(sqlData);
			else
				*(uint8_t *)(userbuf) = 0;//uint8_t

		}
		break;
	case DB_WORD:
		{
			if(sqlData > 0)
				*(uint16_t *)(userbuf) = (uint16_t)atoi(sqlData);
			else
				*(uint16_t *)(userbuf ) = 0;//uint16_t

		}
		break;
	case DB_DWORD:
		{
			if(sqlData > 0)
				*(uint32_t *)(userbuf) = (uint32_t)atoi(sqlData);
			else
				*(uint32_t *)(userbuf ) = 0L;

		}
		break;
	case DB_QWORD:
		{
			if(sqlData > 0)
			{
				*(int64_t *)(userbuf) = (int64_t)strto64_q(sqlData, NULL, 10);
			}
			else
			{
				*(int64_t *)(userbuf) = 0LL;//int64_t
			}


		}
		break;
	case DB_FLOAT:
		{
			if(sqlData > 0)
				*(float *)(userbuf ) = atof(sqlData);
			else
				*(float *)(userbuf ) = 0.0;

		}
		break;
	case DB_DOUBLE:
		{
			if(sqlData > 0)
				*(double *)(userbuf ) = atof(sqlData);
			else
				*(double *)(userbuf ) = 0.0;

		}
		break;
	case DB_RAWSTR:
	case DB_STR:
		{
			if(dwuserMaxlen > 0)
			{
				ZeroMemory(userbuf , dwuserMaxlen);

				if(sqlData> 0)
				{
					strcpy_q(userbuf , dwuserMaxlen, sqlData);
				}
			}
		}
		break;
	case DB_DATETIME:
		{
			if(sqlData > 0)
				*(uint32_t *)(userbuf) = strtotime_q(sqlData);
			else
				*(uint32_t *)(userbuf) = 0;
		}
		break;
	case DB_BIN:
		{
			if(dwuserMaxlen > 0)
			{
				ZeroMemory(userbuf , dwuserMaxlen);

				//            if((sqlData) != 0)
				//            {
				//                // SafeArrayAccessData(sqldata.parray, (void **)&pBuf);
				//            }

				if(sqlData  > 0)
				{
					memcpy(userbuf,sqlData,dwuserMaxlen);
					//bcopy(userbuf, userbuf + offset, dwuserlen > dwsqlLen ? dwsqlLen : dwuserlen);
					//SafeArrayUnaccessData(sqldata.parray);
				}

			}
		}
		break;
		//    case DB_BIN2:
		//    {
		//        *((uint32_t *)(userbuf + offset)) = 0;
		//        uint32_t bin2size = sizeof(uint32_t);
		//
		//        if((sqldata.vt & VT_ARRAY) != 0)
		//        {
		//            //SafeArrayAccessData(sqldata.parray, (void **)&pBuf);
		//        }
		//
		//        if(pBuf && dwsqlLen >= sizeof(uint32_t))
		//        {
		//            bin2size = *((uint32_t *)pBuf);
		//
		//            if(dwuserlen != 0 && bin2size > dwuserlen)
		//            {
		//                g_logger.error("[ %s ] %s size large( %d > %d )...", __FUNC_LINE__ , getTypeString(_etype), bin2size , dwuserlen);
		//                offset += sizeof(uint32_t);
		//                //SafeArrayUnaccessData(sqldata.parray);
		//                return eDataSqlUserRetSizeError;
		//            }
		//
		//            bin2size += sizeof(uint32_t);
		//            int truebinsize = (bin2size > dwsqlLen ? dwsqlLen : bin2size);
		//            ZeroMemory(userbuf + offset, truebinsize);
		//            bcopy(pBuf, userbuf + offset, truebinsize);
		//            *((uint32_t *)(userbuf + offset)) = truebinsize - sizeof(uint32_t);
		//            SafeArrayUnaccessData(sqldata.parray);
		//        }
		//
		//        offset += bin2size;
		//    }
		//    break;
		//    case DB_ZIP:
		//    case DB_ZIP2:
		//    {
		//        if(!(dwuserlen == 0 && _etype == DB_ZIP))
		//        {
		//            uint32_t bin2size = 0;
		//
		//            if(_etype == DB_ZIP)
		//            {
		//                ZeroMemory(userbuf + offset, dwuserlen);
		//                bin2size = dwuserlen;
		//            }
		//            else
		//            {
		//                *((uint32_t *)(userbuf + offset)) = 0;
		//                bin2size = sizeof(uint32_t);
		//            }
		//
		//            if((sqldata.vt & VT_ARRAY) != 0)
		//            {
		//               // SafeArrayAccessData(sqldata.parray, (void **)&pBuf);
		//            }
		//
		//            if(pBuf && dwsqlLen > 0)
		//            {
		//                int retcode;
		//                uLong destLen = dwuserlen;
		//
		//                if(_etype == DB_ZIP2 && dwuserlen == 0)
		//                {
		//                    destLen = 0x7fffffff;
		//                }
		//
		//                retcode = uncompress((Bytef *)userbuf + offset, &destLen, (Bytef *)pBuf, dwsqlLen);
		//
		//                switch(retcode)
		//                {
		//                case Z_OK:
		//                {
		//                    if(_etype == DB_ZIP2 && destLen >= sizeof(uint32_t))
		//                    {
		//                        bin2size = *((uint32_t *)(userbuf + offset));
		//
		//                        if(dwuserlen != 0 && bin2size > dwuserlen)
		//                        {
		//                            g_logger.error("[ %s ] %s size large( %d > %d )...", __FUNC_LINE__ , getTypeString(_etype), bin2size , dwuserlen);
		//                            *((uint32_t *)(userbuf + offset)) = 0;
		//                            offset += sizeof(uint32_t);
		//                            //SafeArrayUnaccessData(sqldata.parray);
		//                            return eDataSqlUserRetSizeError;
		//                        }
		//
		//                        bin2size += sizeof(uint32_t);
		//                    }
		//
		//                    if(bin2size != destLen)
		//                    {
		//                        g_logger.error("[ %s ] %s uncompress data error( %d<>%d )...", __FUNC_LINE__ , getTypeString(_etype), bin2size, destLen);
		//
		//                        if(_etype == DB_ZIP)
		//                        {
		//                            ZeroMemory(userbuf + offset, dwuserlen);
		//                            bin2size = dwuserlen;
		//                        }
		//                        else
		//                        {
		//                            *((uint32_t *)(userbuf + offset)) = 0;
		//                            bin2size = sizeof(uint32_t);
		//                        }
		//
		//                        offset += bin2size;
		//                       // SafeArrayUnaccessData(sqldata.parray);
		//                        return eDataSqlUserRetSizeError;
		//                    }
		//                }
		//                break;
		//                case Z_MEM_ERROR:
		//                case Z_BUF_ERROR:
		//                case Z_DATA_ERROR:
		//                {
		//                    ZeroMemory(userbuf + offset, bin2size);
		//                    offset += bin2size;
		//                   // SafeArrayUnaccessData(sqldata.parray);
		//                    return  eDataSqlUserRetSizeError;
		//                }
		//                }
		//
		//               // SafeArrayUnaccessData(sqldata.parray);
		//            }
		//
		//            offset += bin2size;
		//        }
		//    }
		//    break;
	default:
		{
			//        g_logger.error("[ %s ] %s invalid type...", __FUNC_LINE__ , getTypeString(_etype));
			return true;
		}
	}

	return 1;
}

int CSqlBase::getinsertid()
{
    FUNCTION_BEGIN;
   // bool m_bocangetqinid = false;
   // int m_qinid = (unsigned int )-1;
  	if(m_bocangetqinid)
	{
		return m_qinid;
	}

	if(getaffectedrows() >= 0)
	{
       #define SELECT_IDENTITY			"SELECT @@IDENTITY AS RETIID;"
       //#define SELECT_IDENTITY			"SET NOCOUNT ON;SELECT SCOPE_IDENTITY() AS RETIID;"
		dbCol myinto_define[] =
		{
			{ _DBC_SA_("RETIID", DB_DWORD, m_qinid) },
			{_DBC_NULL_}
		};
		int64_t nrcode = execSelectSql(SELECT_IDENTITY, myinto_define, (unsigned char *)(&m_qinid), sizeof(m_qinid), sizeof(SELECT_IDENTITY) - 1);
		m_bocangetqinid = true;

		if(nrcode <= 0 || m_qinid <= 0)
		{
			m_bocangetqinid = false;
			m_qinid = (unsigned int) - 1;
		}
	}

	return m_qinid;
}

unsigned int CSqlBase::SqlProcessThread( )
{
	if (m_MyPool)
	{
		CCONDSIGAL_PUSHCLEAN(m_ProcessCondLock);

		while (true)
		{

			if (!m_sqllist.IsFull())
			{
				if (m_MyPool)
				{
					INFOLOCK(m_ProcessCondLock.getCIntLock());
					while (/*m_MyPool->m_sqlCacheIssue.size() <= 0 ||*/ m_Active == false)//阻塞等待
					{
						m_ProcessCondLock.waitEvent();
					}
					UNINFOLOCK(m_ProcessCondLock.getCIntLock());

					if (m_MyPool->m_sqlCacheIssue.size() > 0)
					{
						AILOCKT(m_MyPool->m_sqlCacheIssue);
						m_sqllist.merge(m_MyPool->m_sqlCacheIssue);
					}
				}
			}

			uint32_t dwNow2 = GetTickCount32_Q();
			if (m_sqllist.size() > 0 )
			{
				int nSqlCount = 0;

 				AILOCKT(m_sqlLock);
				do
				{
					//处理sql消息
					stSqlOperation* pSql = m_sqllist.pop_front();
					if (pSql)
					{
						if (!bExecuteSql(pSql->szSqlStr))
						{
							continue;
						}

						nSqlCount++;


						if (pSql->dwHashCode > 0)//是否需要结果集
						{
							MYSQL_RES *result = mysql_store_result(m_mysql);
							if (result && pSql->col)
							{
								int32_t num_fields = mysql_num_fields(result);//获得列数
								int64_t num_Effect = mysql_affected_rows(m_mysql);
								if (num_fields > 0)
								{
									m_MyPool->AddSqlRestult(pSql->dwHashCode,num_Effect,pSql->col,result);
								}
								else
								{
									m_MyPool->AddSqlRestult(pSql->dwHashCode,num_Effect,NULL,NULL);
									mysql_free_result(result);
								}
							}
						}
					}
				}
				while(m_sqllist.size() > 0);

				m_Time += GetTickCount32_Q() - dwNow2;
				n_Count += nSqlCount;
				m_NowTime = time(NULL);
				//  				g_logger.debug("sql操作(%d/%d)条,执行时间(%d),执行线程(%u),当前时间(%d)",nSqlCount,m_sqllist.size(),::GetTickCount() - dwNow,p->getThreadId(),time(NULL));
			}
			Sleep_Q(2);
	// 		if (m_MyPool && m_MyPool->m_sqlCacheIssue.size() > 0 )
	// 		{
	// 			Sleep(2);
	// 		}
	// 		else
	// 		{
	// 			Sleep(_SQL_TickTime_);
	// 		}
		}
		CCONDSIGAL_POP;
	}

	return 1;
}

void CSqlBase::setMyPool(SqlConnPool*	Pool)
{
	m_MyPool = Pool;
}
#endif
