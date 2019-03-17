#ifndef _SQL_DATA_DEFINE_H__
#define _SQL_DATA_DEFINE_H__

#include"define/define.h"
#include"serialize/packet.h"

/*************************************************************
* 说明 : db列结构
*************************************************************/
#define _DBCOL_NOT_READ_		1
#define _DBCOL_NOT_WRITE_		2
#define _DBCOL_NOT_CHECK_NAME_	4
#define _DBCOL_WRITENAMEBACK_	8


enum eSD2U_RC
{
    eDataSqlUserRetTypeError = -2,
    eDataSqlUserRetOther = -1,
    eDataSqlUserRetOk = 0,
    eDataSqlUserRetSizeError = 1,
};

/*列类型*/
enum eDB_CLDTYPE
{
    DB_TYPEBEGIN,//0
    DB_BYTE,
    DB_WORD,
    DB_DWORD,
    DB_QWORD,
    DB_FLOAT,
    DB_DOUBLE,
    DB_RAWSTR,
    DB_STR,
    DB_DATETIME,
    DB_BIN,
    DB_ZIP,
    DB_BIN2,
    DB_ZIP2,
    DB_TYPEEND,
};

struct dbCol
{
    const char*		name;//数据库里面的字段名称
    uint16_t		type;//uint16_t
    uint8_t			state;//uint8_t
    uint8_t			reserva;//uint8_t
    uint32_t	    type_size;//unsigned int
    uint32_t	    data_offset;//unsigned int
    uint8_t*	    data_addr;//unsigned char
    uint16_t		dbcolsize;//uint16_t
    char			szNameBuffer[64];
    //unsigned int    nStSize;//这个结构体的大小

    static const dbCol* findbyName(const dbCol* pfirstdbcol, const char* szName)
    {
        if(pfirstdbcol && szName)
        {
            while(pfirstdbcol->name)
            {
                if(strcmp(pfirstdbcol->name, szName) == 0)
                {
                    return pfirstdbcol;
                }

                pfirstdbcol++;
            }
        }

        return NULL;
    }
    __inline bool canRead() const
    {
        return ((state & _DBCOL_NOT_READ_) == 0);
    }
    __inline bool canWrite() const
    {
        return ((state & _DBCOL_NOT_WRITE_) == 0);
    }
    __inline bool checkName() const
    {
        return ((state & _DBCOL_NOT_CHECK_NAME_) == 0);
    }
    bool writeNameBack(const char* szname)
    {
        if(((state & _DBCOL_NOT_CHECK_NAME_) != 0 && (state & _DBCOL_WRITENAMEBACK_) != 0))
        {
            name = szNameBuffer;
            strcpy_q(szNameBuffer, sizeof(szNameBuffer) - 1, szname);
            return true;
        }

        return false;
    }
    void clone(const dbCol* pcol)
    {
        CopyMemory(this, pcol, sizeof(*this));

        if(pcol->name)
        {
			memset(szNameBuffer, 0, sizeof(szNameBuffer));
            strcpy_q(szNameBuffer, sizeof(szNameBuffer) - 1, pcol->name);
            name = szNameBuffer;
        }
        else
        {
           memset(szNameBuffer, 0, sizeof(szNameBuffer));
            name = NULL;
        }
    }
} ;

//包装dbcol结构 用于存储表字段
struct dbColProxy
{
	uint8_t buffer[sizeof(dbCol)];//uint8_t
	dbColProxy()
	{
	    memset(this,0,sizeof(*this));
	}
	dbCol* getdbCol()
	{
		return ((dbCol*)&buffer);
	}
};


#define _DBCOL_SIZE_OFFSET_FULL_(name,dbtype,type,member,state,dbcolsize)	name,dbtype,state,0,sizeof( ((type*)0)->member ), (size_t)(&((type*)0)->member),NULL,dbcolsize,""
#define _DBCOL_SIZE_OFFSET_(name,dbtype,type,member)	_DBCOL_SIZE_OFFSET_FULL_(name,dbtype,type,member,0,sizeof(dbCol))

#define _DBC_SO_		_DBCOL_SIZE_OFFSET_
#define _DBC_SOF_		_DBCOL_SIZE_OFFSET_FULL_

#define _DBCOL_SIZE_ADDR_FULL_(name,dbtype,member,state,dbcolsize)		name,dbtype,state,0,sizeof(member),0,(unsigned char *)&(member),dbcolsize
#define _DBCOL_SIZE_ADDR_(name,dbtype,member)		_DBCOL_SIZE_ADDR_FULL_(name,dbtype,member,0,sizeof(dbCol))

#define _DBC_SA_		_DBCOL_SIZE_ADDR_
#define _DBC_SAF_		_DBCOL_SIZE_ADDR_FULL_    //针对一个静态变量 地址

#define _DBC_NULL_		NULL,0,0,0,0,0,NULL,sizeof(dbCol)

#define _DBC_NULL_MAXOFFSET_(type)		NULL,0,0,0,sizeof(type),0,NULL,sizeof(dbCol)
#define _DBC_MO_NULL_	_DBC_NULL_MAXOFFSET_





#endif // _SQL_DATA_DEFINE_H__
