
#ifndef _MD5_H__
#define _MD5_H__


#ifndef UINT4
typedef unsigned int UINT4;
#endif

typedef unsigned char MD5_DIGEST[16];

typedef struct
{
	UINT4 i[2];
	UINT4 buf[4];
	unsigned char in[64];
	MD5_DIGEST digest;
} MD5_CTX;


void MD5_Transform(UINT4 *buf, UINT4 *in);

void MD5Init(MD5_CTX *mdContext, unsigned int pseudoRandomNumber = 0);
void MD5Update(MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen);
void MD5Final(MD5_CTX *mdContext, MD5_DIGEST* pMd5 = 0);

#endif //_MD5_H__
