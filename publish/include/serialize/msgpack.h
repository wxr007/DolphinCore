/*------------- sockettask.h
*
* Copyright (C): www.7cool.cn (2010)
* Author       :
* Version      : V1.01
* Date         : 2013/06/03 15:50:02
*
*/
/*************************************************************
*与逻辑层相关的，例如：加密解密，组包封包
*************************************************************/
#ifndef _MSG_PACK_H__
#define _MSG_PACK_H__


#include "platform/platform.h"
#include "serialize/packet.h"
#include "mem/streamqueue.h"
#include "mem/loopBuf.h"
#include "utils/string/stringex.h"
//#include "CMisc.h"
//#include "Logger.h"


//#include"global.h"
/*************************************************************/
class CEncrypt;
//struct stQueueMsgParam;
#define Z_DEFAULT_COMPRESSION  (-1)


#define  _DEF_VALID_TIMEOUT_				5
#define  _DEF_CHECK_SIGNAL_INTERVAL_		60*4+rand()%(60*4)

#ifdef   _USE_API_LOADLIB_
#define  _DEF_CHECK_SIGNAL_WAITTIME_		60*120
#else
#define  _DEF_CHECK_SIGNAL_WAITTIME_		30
#endif



extern unsigned char connect_def_key_16_byte[16];
//////////////////////////////////////////////////////////////////////////
//检查连接是否正常 cmd=255  subcmd=254
#define _CHECK_SIGNAL_CMD_					0xff
#define _CHECK_SIGNAL_SUBCMDCMD_			0xfe
struct stCheckSignalCmd : public stCmdBase<_CHECK_SIGNAL_CMD_, _CHECK_SIGNAL_SUBCMDCMD_>
{
    bool isneedACK;
    uint8_t checknum;//uint8_t
    stCheckSignalCmd(bool isfirstsend, uint8_t ch = 0): isneedACK(isfirstsend), checknum(ch) {};
    stCheckSignalCmd(const stCheckSignalCmd* psrc): isneedACK(false), checknum((psrc != NULL) ? psrc->checknum : 0) {};
};

#define _CHECK_SPEED_SUBCMDCMD_			0xfd
struct stCheckSpeedCmd: public stCmdBase<_CHECK_SIGNAL_CMD_, _CHECK_SPEED_SUBCMDCMD_>
{
    int64_t dwLocalTick;//__int64
    uint32_t dwCheckIndex;
    uint32_t dwProxyCount;
    uint32_t dwCurtime;
    char szCheckStr[128];
#define _MAX_LOG_COUNT_		16
    struct
    {
        uint32_t svr_id_type;
        uint32_t dwLoaclTick;
        uint32_t dwCurtime;
    } ProcessLogs[_MAX_LOG_COUNT_];
    stCheckSpeedCmd(uint32_t dwidx, const char* pszCS)
    {
        dwCheckIndex = dwidx;
        strcpy_q(szCheckStr, sizeof(szCheckStr) - 1, pszCS);
        dwLocalTick = GetTickCount64_Q();
    }
    void setLog(uint32_t svr_it)
    {
        dwProxyCount++;
        return;
    }
};

enum eCheckSignalState
{
    checksignalstate_no,//0
    checksignalstate_waitrecv,
	checksignalstate_getrecv,
};

enum TerminateMethod
{
    terminate_no,
    terminate_client_active,
    terminate_server_active,
};

struct stQueueMsg
{
    uint32_t cmdsize;
    uint32_t pluscmdoffset;
    stBaseCmd cmdBuffer;

    __inline stBaseCmd* pluscmd()
    {
        return (stBaseCmd*)((int64_t)(&cmdBuffer) + pluscmdoffset);
    }
    __inline uint32_t pluscmdsize()
    {
        return (cmdsize - pluscmdoffset);
    }
};

struct stQueueMsgParam
{
    stQueueMsg* pQueueMsgBuffer;
    bool bofreebuffer;
    stQueueMsgParam(stQueueMsg* p, bool bo):  pQueueMsgBuffer(p),bofreebuffer(bo) {};
};

template < class _RET >
__inline bool NewPacketBuffer(_RET*& msg, size_t size)
{
    return CSafeMsgQueue::NewPushBuffer(msg, size);
};

template < class _RET >
__inline bool FreePacketBuffer(_RET*& msg)
{
    return CSafeMsgQueue::FreePushBuffer(msg);
};

int  getmsg2buf(const char*& pbuf, unsigned int& nbuflen, CEncrypt* enc, unsigned int& _de_size, stBaseCmd* pmsg, int nmsgmaxlen);
int  packetbuf(unsigned char* pin, uint64_t ninlen, unsigned char* pout, uint64_t nmaxlen, CEncrypt* enc, int zliblevel, bool issplit);

extern _TH_VAR_ARRAY(char, tls_sendpacket_charbuffer, _MAX_SEND_PACKET_SIZE_ + 1);
__inline int getsafepacketbuflen()
{
    return _TH_VAR_SIZEOF(tls_sendpacket_charbuffer);
};
__inline char* getsafepacketbuf()
{
    return (char*) _TH_VAR_PTR(tls_sendpacket_charbuffer);
};




int packet_getcmd(LoopBuf* pbuf, CEncrypt* enc, unsigned int& _de_size,
                  stBaseCmd* pmsg, int nmsgmaxlen);
// bool packet_addcmd(CEpollObj& po, CEncrypt* enc, void* pbuf, unsigned int nsize,
//                    int zliblevel);
stBasePacket* getpackethdr(const char* pbuf, unsigned int nbuflen,
                           CEncrypt* enc, unsigned int& _de_size, bool& isfullpacket,
                           bool& iserror);

#endif //_MSG_PACK_H__