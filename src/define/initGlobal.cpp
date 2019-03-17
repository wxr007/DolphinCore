//控制全局变量初始化顺序


#include "utils/string/stringex.h"
#include "serialize/packet.h"
#include "mem/frameallocator.h"
#include "logging/logging/zLogger.h"


CSyncVector<zLogger*> zLogger::m_loggers;//m_loggers的初始化必须早与 g_logger 因为g_logger构造函数中调用了 m_loggers

zLogger g_logger(std::string(""));//如果g_logger 和 m_loggers 初始化顺序 反过来的话 则必然会出错
zLogger chatlogger(std::string(""));
zLogger g_onlineuserlogger;

//sockettask.cpp
_TH_VAR_INIT_ARRAY(char, tls_sendpacket_charbuffer, _MAX_SEND_PACKET_SIZE_ + 1, '\0');
_TH_VAR_INIT_ARRAY(char, tls_packetbuf_charbuffer, _MAX_SEND_PACKET_SIZE_ + 1, '\0');//线程变量



//gatewaySvrSession.cpp  gatewayclientSession.cpp
_TH_VAR_INIT_ARRAY(char, tls_gatewayproxydata_charbuffer, _MAX_SEND_PACKET_SIZE_ + 1, '\0');

_TH_VAR_INIT_ARRAY(char, tls_loop_charbuffer, MAX_TLS_LOOPCHARBUFFER + 1, '\0');




map<void*, stFuckTest> g_mpNewData;
map<void*, stFuckTest> g_mpDefData;


uint64_t stStackFrameAllocator::m_initframeSize = 0;

stStackFrameAllocator init_tls_FrameAllocator = { 0, NULL, { 0 } };
_TH_VAR_INIT(stStackFrameAllocator, tls_FrameAllocator, init_tls_FrameAllocator);


// stThreadObjInfo init_tls_CurrThreadObj = { NULL, 0, 0 };
// _TH_VAR_INIT(stThreadObjInfo, tls_CurrThreadObj, init_tls_CurrThreadObj);

