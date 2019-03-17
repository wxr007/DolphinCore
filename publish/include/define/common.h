
#ifndef _COMMON_H__
#define _COMMON_H__

#include <cstring>
#include <cerrno>
#include <stdlib.h>//for malloc;
#include <stdint.h>
//0-for 64int printf;
#ifndef WIN32
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#else
# define PRIi64 "lld"
# define PRIu64 "llu"
# define PRIx64 "llx"
#endif

//1-type_cast;
//主要用于port转换(int32_t-->int16_t).
#define INT16(V) static_cast<int16_t>(V)
#define UINT16(V) static_cast<uint16_t>(V)
//主要用于枚举类型转换;
#define INT32(V) static_cast<int32_t>(V)
#define UINT32(V) static_cast<uint32_t>(V)

#define MASK4BIT  0xfL
#define MASK8BIT  0xffL
#define MASK16BIT 0xffffL
#define MASK32BIT 0xffffffffL

//2-for network define;
typedef uint64_t LinkKey;
const LinkKey INVALID_LINKKEY = -1;
inline LinkKey SETLINKKEY(int64_t objid,int64_t id)
{
    return (objid << 32 )| (id &0xffffffff);
}
#define GETOBJECTID(LINKKEY) INT32((LINKKEY>>32)&MASK32BIT)
//#define GETLINKID(LINKKEY) (LINKKEY)&MASK32BIT

//TcpMessage::type_定义.
// const int32_t TM_TYPE_NET_DATA = 0;//默认,msg存放的是net数据;
// const int32_t TM_TYPE_CLI_START_FAILED = 1;
// const int32_t TM_TYPE_SVR_START_FAILED = 2;
// const int32_t TM_TYPE_LINK_ADD = 3;
// const int32_t TM_TYPE_LINK_DEL = 4;
//----------------------------------------------------------
//TcpMessage::type_定义.
//说明:TM_TYPE_NET_DATA:数据包;
//     TM_TYPE_CLI_CONNECT_OK/TM_TYPE_SVR_ACCEPT_OK:对应OnConnect;
//     others:表示发生错误,需要删除该链接.
const int32_t TM_TYPE_NET_DATA = 0;//默认,msg存放的是net数据;

const int32_t TM_TYPE_CLI_START_FAILED = 0x11;//调用socket函数失败;
const int32_t TM_TYPE_CLI_ERROR = 0x19;//
const int32_t TM_TYPE_CLI_CONNECT_FAILED = 0x12;//调用connect函数失败;
const int32_t TM_TYPE_CLI_CONNECT_OK = 0x10;//调用connect函数成功;

const int32_t TM_TYPE_SVR_START_FAILED = 0x21;//调用socket/bind/listen函数失败;
const int32_t TM_TYPE_SVR_ERROR = 0x20;//
const int32_t TM_TYPE_SVR_ACCEPT_FAILED = 0x22;//调用accept函数失败;
const int32_t TM_TYPE_SVR_ACCEPT_OK = 0x23;//调用accept函数成功;

//const int32_t TM_TYPE_LINK_REG_RD_CB_ERROR = 0x31;//注册read回调函数失败;
//const int32_t TM_TYPE_LINK_REG_WR_CB_ERROR = 0x32;//注册write回调函数失败;
const int32_t TM_TYPE_LINK_ERROR = 0x33;//IOCP进行read/write失败;
const int32_t TM_TYPE_SYSTEM_STOP = 0x30;//主动停服务，踢人./stopServer/stopclient



#include "mem/qmemory.h"
struct TcpMessage
{
    LinkKey key_;
    int32_t type_;
    int32_t dlen_;
    void *data_;

    TcpMessage(int32_t t = 0):key_(0),type_(t){clear();}

    void clear()
    {
        dlen_ = 0;
        data_ = NULL;
    }
    //实际的数据包长度;这里不会进行字节对齐.
    void data_new(int32_t sz)
    {
        data_ = qmalloc(sz);
        dlen_ = sz;
    }
    void data_free()
    {
        //type_++;
        if (data_)
        {
            qfree(data_);
            clear();
        }
    }
};

//3-comon base class;
class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:  // emphasize the following members are private
    noncopyable( const noncopyable& );
    const noncopyable& operator=( const noncopyable& );
};

//4-common base function;
inline void bzero(void *d,size_t sz)
{
    memset(d,0,sz);
}

#define ROUNDNUM8(DLEN)     ( ((DLEN) + (8-1)) & (~(8-1)) )    // (len + 7）& ~7

//4-业务相关声明.

#ifdef _WIN32
inline void SetThreadName(const char *name)
{
}
#else
#include <sys/prctl.h>
inline void SetThreadName(const char *name)
{
    if (name)
    {
        ::prctl(PR_SET_NAME,name);
    }
}
#endif

//
enum LinkAttrType
{
    LAT_NONE    = 0,
    LAT_GATEWAY = 1,
};
typedef int32_t LinkAttr;

const int32_t GW_AUTHORITY_SIZE = 256;
const char GW_AUTHORITY_PACKATE[] = "tgw_l7_forward\r\nHost:";
struct GWConnecter
{
    char buf_[256];
    int32_t write_;
    bool is_auth_;
    GWConnecter():write_(0),is_auth_(false){}
};


#endif //_COMMON_H__
