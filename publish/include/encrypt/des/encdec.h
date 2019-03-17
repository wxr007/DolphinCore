#ifndef _ENCDEC_H__
#define _ENCDEC_H__

#include <stdlib.h>
#include "encrypt/des/des.h"

//typedef int DES_cblock;
//typedef const int const_DES_cblock;
#define  RC5_8_ROUNDS  8

class CEncrypt
{
public:
	CEncrypt();
	enum encMethod{
		ENCDEC_NONE=0,
		ENCDEC_RC5=1,
		ENCDEC_DES=2,
		ENCDEC_FORCE_DWORD=0x7fffffff,
	};
	void random_key_des(DES_cblock *ret);
	void set_key_des(const_DES_cblock *key);
	void set_key_rc5(const unsigned char *data, int nLen, int rounds = RC5_8_ROUNDS);


	int encdec(void *data, unsigned int nLen, bool enc);

	void setEncMethod(encMethod method);
	encMethod getEncMethod() const;

private:
	void DES_encrypt1(DES_LONG *data, DES_key_schedule *ks, int enc);
	int encdec_des(unsigned char *data, unsigned int nLen, bool enc);

	encMethod method;

	DES_key_schedule key_des;
	bool haveKey_des;

};

#endif//_ENCDEC_H__

