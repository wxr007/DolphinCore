#include "encrypt/des/encdec.h"
#include <string.h>
//#include "dump/DumpErrorBase.h"

#ifdef _WIN32
#define bzero(a,b)  ZeroMemory((a),(b))
#endif

CEncrypt::CEncrypt()
{
	method = ENCDEC_NONE;
}

void CEncrypt::random_key_des(DES_cblock *ret)
{
	return;
}

extern void DES_set_key(const_DES_cblock *key, DES_key_schedule *schedule);
void CEncrypt::set_key_des(const_DES_cblock *key)
{
	::DES_set_key(key, &key_des);
	haveKey_des = true;
	return;
}

void CEncrypt::set_key_rc5(const unsigned char *data, int nLen, int rounds)
{
	return;
}

extern void DES_encrypt1(DES_LONG *data, DES_key_schedule *ks, t_DES_SPtrans * sp, int enc);

void CEncrypt::DES_encrypt1(DES_LONG *data, DES_key_schedule *ks, int enc)
{
	//FUNCTION_BEGIN;
	//m_fDES_encrypt1(data,ks,&MyDES_SPtrans,enc);
	::DES_encrypt1(data, ks, &MyDES_SPtrans, enc);
}

int CEncrypt::encdec_des(unsigned char *data, unsigned int nLen, bool enc)
{
	//FUNCTION_BEGIN;
	if((0 == data) || (!haveKey_des)) return -1;

	unsigned int offset = 0;

	while(offset <= nLen - 8)
	{
		DES_encrypt1((DES_LONG*)(data + offset), &key_des, enc);
		offset += 8;
	}

	return nLen - offset;
}

int CEncrypt::encdec(void *data, unsigned int nLen, bool enc)
{
	if(ENCDEC_NONE == method) return -1;

	if(ENCDEC_DES == method) return encdec_des((unsigned char *)data, nLen, enc);

	return -2;
}

void CEncrypt::setEncMethod(encMethod m)
{
	method = m;
}

CEncrypt::encMethod CEncrypt::getEncMethod() const
{
	return method;
}
