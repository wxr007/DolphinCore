#include"define/define.h"

#include"mem/loopBuf.h"
#include"serialize/packet.h"
#include"logging/logging/zLogger.h"



LoopBuf::LoopBuf(int nBufSize)
{


    m_nAllSize		= nBufSize;
    m_nReadLeft		= 0;        //初始化的时候没有任何数据
    m_nWritLeft		= nBufSize;
    m_nPerpendLeft	= 0;

    //ptr
    m_ptrBegin=new char[nBufSize];
	memset(m_ptrBegin, 0, nBufSize);
    DEFINE_ADD_ONE(m_ptrBegin);
    m_ptrRead	= m_ptrBegin;	//没有数据可读
    m_ptrWrite	= m_ptrBegin;	//首地址
}

LoopBuf::~LoopBuf()
{
	if (m_ptrBegin)
    {
		DEFINE_DEL_ONE(m_ptrBegin);
		SAFE_DELETE_VEC(m_ptrBegin);

    }
}



int LoopBuf::putData(const char* p,int nLen)
{
    //checkRight("putData++++++++++++++++++++++++++++++++++++++++");
    if (p==NULL||nLen<=0)
    {
        return 0;
    }
    //g_logger.debug("m_nWritLeft%d nLen%d",m_nWritLeft,nLen);
    if (m_nWritLeft<nLen)//没有足够的数据写入了
    {
        if (m_nWritLeft+m_nPerpendLeft>(nLen*2))//重组前面空闲的数据
        {
            RestructuringData();//自己内存挪腾
        }
        else
        {

            resizeNewAllData(nLen);//重新分配内存 参数 nLen 是为了保证 分配的内存一定可以容纳 要写入的nLen
        }
    }

    return putRightData(p,nLen);
}

int LoopBuf::putRightData(const char*p,int nLen)
{
//    static int nAllCount=0;
    //g_logger.debug("putRightData m_ptrWrite%x nLen%d",m_ptrWrite,nLen);
	if (m_nWritLeft < nLen)
	{
		g_logger.error("putRightData m_nWritLeft%d nLen%d m_nReadLeft%d m_nAllSize%d",m_nWritLeft,nLen,m_nReadLeft,m_nAllSize);
		return 0;
	}

    if (p==NULL||nLen<=0)
    {
        return 0;
    }
//    nAllCount+=nLen;

    memcpy(m_ptrWrite,p,nLen);
    m_ptrWrite+=nLen;
    m_nWritLeft-=nLen;
    m_nReadLeft+=nLen;
    return nLen;
}

char* LoopBuf::getReadPtr()
{

    if (m_nReadLeft<=0)
    {
        return NULL;
    }
    return m_ptrRead;//这里没数据的时候返回null
}



int LoopBuf::setReadPtr(int nlen)
{

    if (nlen<=0||m_nReadLeft<=0)
    {
        return 0;
    }
    //300000 525 次 挪腾
    if (m_nReadLeft <= nlen)//数据正好读完了
    {
        m_ptrRead	= m_ptrBegin;
        m_ptrWrite	= m_ptrBegin;
        m_nReadLeft	= 0;
        m_nWritLeft	= m_nAllSize;
        m_nPerpendLeft = 0;
       // g_logger.debug("数据正好读完了 剩余数量%d\n",m_nReadLeft);

        return m_nReadLeft;
    }
    m_ptrRead	+= nlen;
    m_nPerpendLeft += nlen;
    m_nReadLeft	-= nlen;

    //g_logger.debug("数据未读完 剩余数量%d\n",m_nReadLeft);
    return nlen;
}

void LoopBuf::RestructuringData()
{

    memmove(m_ptrBegin,m_ptrRead,m_nReadLeft);//可以把自己的一部分拷贝给自己的另一部分 memcpy 则会出现错误
    m_ptrRead	= m_ptrBegin;
    m_ptrWrite	= m_ptrBegin + m_nReadLeft;
    m_nPerpendLeft = 0;
    m_nWritLeft	= m_nAllSize - m_nReadLeft;

    return;
}

void LoopBuf::resizeNewAllData(int ninLen)
{
	//DOTO 现在 m_nAllSize 初始是 3072
	int nAll = ROUNDNUM2((m_nAllSize + ninLen) + m_nAllSize, 1024);	//两倍扩张
	
	if (nAll > DEF_LOOPBUF_MAX_RESIZE)					//WARN 重新分配内存上限设置
	{
		nAll = DEF_LOOPBUF_MAX_RESIZE;
	}

	char *pNew = new char[nAll];
	DEFINE_ADD_ONE(pNew);
	memcpy(pNew,m_ptrRead,m_nReadLeft);

	if (m_ptrBegin)
	{
		DEFINE_DEL_ONE(m_ptrBegin);
		SAFE_DELETE_VEC(m_ptrBegin);
	}

	m_nWritLeft	= nAll-m_nReadLeft;
	m_nAllSize	= nAll;
	m_nPerpendLeft = 0;

	//ptr
	m_ptrBegin	= pNew;
	m_ptrRead	= m_ptrBegin;				//没有数据可读
	m_ptrWrite	= m_ptrBegin + m_nReadLeft;


	//g_logger.debug("重新分配数据%d m_nReadLeft:%d time %d",nAll,m_nReadLeft,dwend-dwstart);
    return;
}

int LoopBuf::getReadDataLen()
{
    return m_nReadLeft;
}
