
#include "serialize/msgpack.h"
#include "logging/logging/zLogger.h"
#include "encrypt/des/encdec.h"
#include "compress/zlib/compress.h"

#define _TWG_LENGTH						256
#define _TWG_AUTHORITY_PACKET			"tgw_l7_forward\r\nHost:"
#define _FLASH_AUTHORITY_POSTSTR_		"<policy-file-request/>"
unsigned char connect_def_key_16_byte[16] = { 19, 98, 167, 15, 243, 192, 199,
        115, 140, 152, 147, 143, 217, 188, 76, 130
                                            };
bool g_boshowproxylog = false;

stBasePacket* getpackethdr(const char* pbuf, unsigned int nbuflen,
                           CEncrypt* enc, unsigned int& _de_size, bool& isfullpacket,
                           bool& iserror)
{
    FUNCTION_BEGIN
    ;
    isfullpacket = false;
    iserror = false;
    stBasePacket* pretpackethdr = (stBasePacket*) pbuf;
	if(nbuflen>=8)
	{
		if((pretpackethdr->size >= 1024 *1024*10) || (pretpackethdr->size < 2))
		{
			g_logger.error("[错误]:受到恶意攻击 %x 字节",pretpackethdr->size);
			iserror = true;
			return NULL;
		}
	}

    if (_de_size == 0 && nbuflen >= 8)
    {
        if (enc && enc->getEncMethod() != CEncrypt::ENCDEC_NONE)
        {
            enc->encdec((void *) &pbuf[_de_size], 8, false);
        }
        _de_size += 8;
    }
    if (_de_size >= 8 && nbuflen >= 8)
    {
        if (pretpackethdr->isfullpacket(nbuflen))//包剩余的长度大于 当前包大小的话 必然是完整的包
        {
            isfullpacket = true;
        }
        return pretpackethdr;
    }
    else
    {
        return NULL;
    }
}

int getmsg2buf(const char*& pbuf, unsigned int& nbuflen, CEncrypt* enc,
               unsigned int& _de_size, stBaseCmd* pmsg, int nmsgmaxlen)
{
    FUNCTION_BEGIN;
    if (nbuflen == 0 || pbuf == NULL)
    {
        return 0;
    }
    uint64_t nlen = nbuflen;//这次收到的数据包的长度
    const char* pbuffer = pbuf;

    if (_de_size > nlen || nmsgmaxlen < (int) sizeof(stBaseCmd))
    {
        return -1;
    }

    unsigned int size = nlen - _de_size;
    stBasePacket* ppacket = (stBasePacket*) pbuffer;
    if (_de_size < 8 || !ppacket->isfullpacket(_de_size))
    {
        if (size < 8)
        {
            return 0;
        }
        else if (_de_size == 0)
        {
            if (enc && enc->getEncMethod() != CEncrypt::ENCDEC_NONE)
            {
                enc->encdec((void *) &pbuffer[_de_size], 8, false);
            }
            _de_size += 8;
            size -= 8;
        }
        size = safe_min(size,
                        ROUNDNUM2(ppacket->getpacketsize(), 8) - _de_size);
        if (size >= 8)
        {
            if (size > 8)
            {
                size = ROUNDNUM2(size-7,8);
                ;
            }
            if (enc && enc->getEncMethod() != CEncrypt::ENCDEC_NONE)
            {
                enc->encdec((void *) &pbuffer[_de_size], size, false);
            }
            _de_size += size;
        }
        if (ppacket->size > (uint32_t) nmsgmaxlen)
        {
            return -2;
        }
        if (!ppacket->isfullpacket(_de_size))
        {
            return 0;
        }
    }

    nlen = ROUNDNUM2(ppacket->getpacketsize(), 8);//实际发包的大小 因为是8个字节对齐的 所以要还原
    nbuflen -= nlen;
    pbuf += nlen;//计算偏移 计算下一个包的偏移地
    _de_size -= nlen;

    uint64_t ncmdlen=nmsgmaxlen;//WARN 这里原来ncmdlen没有初始化 造成 zlib解压bug
    if (ppacket->iscompress())
    {

        if (uncompresszlib((unsigned char *) ppacket->cmd(), ppacket->size, (unsigned char *) pmsg, ncmdlen) != Z_OK
                || ncmdlen > (uint32_t)nmsgmaxlen)
        {
            return -3;
        }


    }
    else
    {

        ncmdlen = ppacket->size;
        CopyMemory(pmsg, ppacket->cmd(), ppacket->size);
    }
    return ncmdlen;
}

int packetbuf(unsigned char* pin, uint64_t ninlen, unsigned char* pout,
              uint64_t nmaxlen, CEncrypt* enc, int zliblevel, bool issplit)
{
    FUNCTION_BEGIN
    ;

    if (ninlen == 0 || pin == NULL || pout == NULL)
    {
        return 0;
    }
    if (ninlen > nmaxlen)
    {
        return -1;
    }
    if (nmaxlen < (ninlen + _MIN_PACKETBUF_SIZE_))
    {
        return -2;
    }

    stBasePacket* ppacket = (stBasePacket*) pout;
    ppacket->init( ( ( ( zliblevel>=Z_BEST_SPEED ) || ( zliblevel==Z_DEFAULT_COMPRESSION ) ) && (ninlen > MINZLIBPACKSIZE) ),issplit );
    if ( ppacket->iscompress() )
    {
        uint64_t nzlibbuflen=nmaxlen;
        if (compresszlib(pin, ninlen, (unsigned char *)ppacket->cmd(), nzlibbuflen, safe_min(((int)zliblevel), ((int)Z_BEST_COMPRESSION))) != Z_OK
                || (nzlibbuflen>nmaxlen) )
        {
            return -3;
        }
        ppacket->size = nzlibbuflen;//uint16_t
        ppacket->setcmdsize(ninlen);
    }
    else
    {
        CopyMemory(ppacket->cmd(), pin, ninlen);
        ppacket->size =  ninlen;//uint16_t
    }


    unsigned int nretsize = ROUNDNUM2(ppacket->getpacketsize(), 8);
    if (nmaxlen < nretsize)
    {
        return -4;
    }
    if (enc && enc->getEncMethod() != CEncrypt::ENCDEC_NONE)
    {
        enc->encdec(ppacket, nretsize, true);
    }
    return nretsize;
}

int packet_getcmd(LoopBuf* pbuf, CEncrypt* enc, unsigned int& _de_size,
                  stBaseCmd* pmsg, int nmsgmaxlen)
{
    unsigned int nlen = pbuf->getReadDataLen();//接收的数据总长度
    if (nlen == 0)
    {
        return 0;
    }
    char* pbuffer = ((char*) pbuf->getReadPtr());
    char* pFuck=pbuffer;
    int ncmdlen = getmsg2buf((const char*&) pbuffer, nlen, enc, _de_size, pmsg,
                             nmsgmaxlen);//包的大小？ 计算下一个包的偏移 并且赋值一个包
    int nFuck=pbuffer-pFuck;
    if (ncmdlen > 0)
    {
        pbuf->setReadPtr(nFuck);
    }
    else if (ncmdlen < 0)
    {
        if (_de_size >= 8)
        {
            // stBasePacket* ptmppacket = (stBasePacket*) pbuffer;

        }
        else
        {

        }
    }
    return ncmdlen;
}
extern _TH_VAR_ARRAY(char,tls_packetbuf_charbuffer,_MAX_SEND_PACKET_SIZE_+1)
;
// bool packet_addcmd(CEpollObj& po, CEncrypt* enc, void* pbuf, unsigned int nsize,
//                    int zliblevel)
// {
// 	char* szzlibbuf = NULL;
// 	szzlibbuf = (char*) _TH_VAR_PTR(tls_packetbuf_charbuffer);
// 	unsigned int nmaxsize = 0;
// 	nmaxsize = _TH_VAR_SIZEOF(tls_packetbuf_charbuffer);
// 
//     if (nsize > nmaxsize)
//     {
//         stBaseCmd* pmsg = (stBaseCmd*) pbuf;
//         g_logger.error("( %d > %d )( %d,%d )", nsize, nmaxsize,
//                        pmsg->cmd, pmsg->subcmd);
//         return false;
//     }
//     else
//     {
//         int nlen = packetbuf((unsigned char*) pbuf, nsize,
//                              (unsigned char *) szzlibbuf, nmaxsize, enc, zliblevel, false);
//         if ((nlen > 0)
//                 && (po.AddSendBuf((const char *) szzlibbuf, nlen) == nlen))
//         {
//             return true;
//         }
//         else
//         {
//             stBaseCmd* pmsg = (stBaseCmd*) pbuf;
//             g_logger.error(" %d (size=%d -> %d,%d)", nlen, nsize,
//                            pmsg->cmd, pmsg->subcmd);
//             return false;
//         }
//     }
// }
