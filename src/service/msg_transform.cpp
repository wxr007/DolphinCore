#include "service/msg_transform.h"
#include "logging/logging/zLogger.h"

namespace MsgTrans
{

bool decodeMessage(stQueueMsg*& logic_msg,TcpMessage& tcp_msg,CEncrypt* penc)
{
    bool hasfullpacket = false;
    bool haserror = false;
    uint32_t _de_size = 0;
    const char* data = (const char*)tcp_msg.data_;
    uint32_t date_len = UINT32(tcp_msg.dlen_);

    stBasePacket* _packet_hdr_ = getpackethdr(data,date_len,penc,_de_size,hasfullpacket,haserror);//CEncrypt 加密暂时Null
    if (haserror)
    {
        return false;
    }
    else if (hasfullpacket)
    {
        int tmpMsgBufferSize = ROUNDNUM2(_packet_hdr_->getcmdsize() + 64, 64);
        if (NewPacketBuffer(logic_msg,tmpMsgBufferSize + sizeof(*logic_msg)))
        {
            int ncmdlen = getmsg2buf(data,date_len, penc,_de_size, &logic_msg->cmdBuffer,tmpMsgBufferSize);//CEncrypt 加密暂时Null
            if (ncmdlen > 0)
            {
                logic_msg->cmdsize = ncmdlen;
                return true;
            }
            else
            {
                FreePacketBuffer(logic_msg);
            }
        }
    }
    return false;
}

_TH_VAR_INIT_ARRAY(char,_package_message_buffer,_MAX_SEND_PACKET_SIZE_+1,0);

bool encodeMessage( void* pbuf, uint32_t nsize,int zliblevel,TcpMessage& tcp_msg,CEncrypt* penc)
{
    char* szzlibbuf = NULL;
    szzlibbuf = (char*) _TH_VAR_PTR(_package_message_buffer);
    uint32_t nmaxsize = 0;
    nmaxsize = _TH_VAR_SIZEOF(_package_message_buffer);
    if (nsize > nmaxsize)
    {
        stBaseCmd* pmsg = (stBaseCmd*) pbuf;
        g_logger.error("( %d > %d )( %d,%d )", nsize, nmaxsize,pmsg->cmd, pmsg->subcmd);
        return false;
    }
    else
    {
        int nlen = packetbuf((unsigned char*) pbuf, nsize,(unsigned char *) szzlibbuf, nmaxsize, penc, zliblevel, false);
        if (nlen > 0)
        {
            tcp_msg.data_new(nlen);
            memcpy(tcp_msg.data_,szzlibbuf,nlen);
            return true;
        }
        else
        {
            stBaseCmd* pmsg = (stBaseCmd*) pbuf;
            g_logger.error(" %d (size=%d -> %d,%d)", nlen, nsize,pmsg->cmd, pmsg->subcmd);
            return false;
        }
    }
}

bool data_encode_to_cache(void* pbuf, uint32_t nsize, int zliblevel,std::string& tcp_cacke, CEncrypt* penc)
{
    char* szzlibbuf = NULL;
    szzlibbuf = (char*)_TH_VAR_PTR(_package_message_buffer);
    uint32_t nmaxsize = 0;
    nmaxsize = _TH_VAR_SIZEOF(_package_message_buffer);
    if (nsize > nmaxsize)
    {
        stBaseCmd* pmsg = (stBaseCmd*)pbuf;
        g_logger.error("data_encode_to_cache:( %d > %d )( %d,%d )", nsize, nmaxsize, pmsg->cmd, pmsg->subcmd);
        return false;
    }
    else
    {
        int nlen = packetbuf((unsigned char*)pbuf, nsize, (unsigned char *)szzlibbuf, nmaxsize, penc, zliblevel, false);
        if (nlen > 0)
        {
            tcp_cacke.append(szzlibbuf, nlen);
            return true;
        }
        else
        {
            stBaseCmd* pmsg = (stBaseCmd*)pbuf;
            g_logger.error("data_encode_to_cache: %d (size=%d -> %d,%d)", nlen, nsize, pmsg->cmd, pmsg->subcmd);
            return false;
        }
    }
}

}