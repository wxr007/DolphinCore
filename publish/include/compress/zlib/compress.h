#ifndef _COMPRESS_H__
#define _COMPRESS_H__

#include <stdint.h>
#include "zlib.h"


int compress (Bytef *dest, uLongf destLen,const Bytef source, uLong sourceLen);
int compresszlib(unsigned char* pIn, uint64_t nInLen, unsigned char* pOut, uint64_t& pOutLen, int level = Z_DEFAULT_COMPRESSION);
int uncompresszlib(unsigned char* pIn, uint64_t nInLen, unsigned char* pOut, uint64_t& pOutLen);


#endif // _COMPRESS_H__
