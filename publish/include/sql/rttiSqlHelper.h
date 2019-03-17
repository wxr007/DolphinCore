#ifndef _RTTISQLHELPER_H__
#define _RTTISQLHELPER_H__

#include <vector>
#include "sql/sqlDataDefine.h"
#include "sql/sqlHelper.h"
#include "utils/rtti/type.h"


class dbColMaker
{
protected:
    std::vector< dbCol > m_dbcols;
public:
    dbColMaker()
    {
        m_dbcols.reserve(50);
    }
    bool put(const char *n, int t, unsigned int ts, unsigned int ndo, unsigned char * da, uint8_t st = 0)
    {
        static dbCol s_tmp = { _DBC_NULL_ };
        dbCol tmp = {n, t, st, 0, ts, ndo, da};
        //tmp.nStSize=stSize;
        if(m_dbcols.size() == 0)
        {
            m_dbcols.push_back(tmp);
        }
        else
        {
            m_dbcols.back() = tmp;
        }

        m_dbcols.push_back(s_tmp);
        return true;
    }
    const dbCol* getdbcol()
    {
        if(m_dbcols.size() > 0)
        {
            return &m_dbcols[0];
        }
        else
        {
            return NULL;
        }
    }
    void clear()
    {
        m_dbcols.clear();
    }
    size_t size()
    {
        return m_dbcols.size();
    }
};

class  CRttiDbDataLoader
{
public:
    static bool MakedbCol(dbColMaker& maker, RTTIClassDescriptor* pclass, int srcdatasize);

    template < class srcdata, class _param >
    static int dbLoad(CSqlBase* sqlc, dbColMaker& maker,
                      const char* sqlstr, int nsrcdatacount, _param param,
                      int srcdatasize = sizeof(srcdata))
    {
        RTTIClassDescriptor* pclass = __RTTITypeOfPtr((srcdata*)NULL);

        if(nsrcdatacount > 0 && pclass && pclass->isClass() && MakedbCol(maker, pclass, srcdatasize) && maker.size() > 0)
        {
            maker.put(0, 0, srcdatasize, 0, 0,0);
            STACK_ALLOCA(char*, srcdatabuffer, ((nsrcdatacount + 1)*srcdatasize));//缓冲区的数据 这里是char 所以 数组 对应位置 偏移 也要注意!!

            if(srcdatabuffer)
            {
                int nret = sqlc->execSelectSql(sqlstr,(dbCol*) maker.getdbcol(), (unsigned char*)srcdatabuffer);

                if(nret > 0)
                {
                    for(int i = 0; i < nret; i++)
                    {
                        if(!srcdata::refresh((srcdata*)&srcdatabuffer[srcdatasize*i], param))//进行数据结构的数据 的更新
                        {
                            return i;
                        }
                    }

                    return nret;
                }
            }
        }

        return 0;
    }

};

#endif // _RTTISQLHELPER_H__
