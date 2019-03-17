#ifndef _MSGTRANFROM_H__
#define _MSGTRANFROM_H__

#include "define/common.h"
#include "serialize/msgpack.h"
#include "encrypt/des/encdec.h"

namespace MsgTrans
{
/*-->[ 解包函数 ]*/
bool decodeMessage(stQueueMsg*& logic_msg,TcpMessage& tcp_msg,CEncrypt* penc = NULL);
/*-->[ 封包函数 ]*/
bool encodeMessage( void* pbuf, uint32_t nsize,int zliblevel,TcpMessage& tcp_msg,CEncrypt* penc = NULL);
/*-->[ 封包函数:使用cache的封包函数 ]*/
bool data_encode_to_cache(void* pbuf, uint32_t nsize,int zliblevel,std::string& tcp_cacke, CEncrypt* penc = NULL);;
}

#endif //_MSGTRANFROM_H__

