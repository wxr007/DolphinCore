/*
* code by wzt base64_encode & decode
*/
#ifndef _BASE64_H__
#define _BASE64_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform/platform.h"

struct stBase64Setup{
	char base64_map[65];
	char base64_mod;
	uint8_t base64_decode_map[256];

	stBase64Setup();

	void init_base64_map(char* pmap,char mod);

	int base64_encode_size(int src_len);
	int base64_decode_size(char *src, int src_len);

	int base64_encode(char *src, int src_len, char *dst,int dst_len);
	int base64_decode(char *src, int src_len, char *dst,int dst_len);

};
extern stBase64Setup g_base64;

int base64_encode(char *src, int src_len, char *dst,int dst_len);
int base64_decode(char *src, int src_len, char *dst,int dst_len);

int base64_encode_size(char *src, int src_len);
int base64_decode_size(char *src, int src_len);
#endif   //_BASE64_H__
