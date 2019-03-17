
#ifndef _CRC32_H__
#define _CRC32_H__
#include <stdint.h>

void crc32Init(uint64_t *pCrc32);
void crc32Update(uint64_t *pCrc32, unsigned char *pData, uint64_t uSize);
void crc32Finish(uint64_t *pCrc32);

inline uint64_t crc32data(unsigned char *pData, uint64_t uSize){
	uint64_t ret;
	crc32Init(&ret);
	crc32Update(&ret,pData,uSize);
	crc32Finish(&ret);
	return ret;
}

#endif //_CRC32_H__
