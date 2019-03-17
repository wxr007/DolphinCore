#include "platform/platform.h"
#include "encrypt/base64/base64.h"


const char def_base64_map[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char def_base64_mod = '=';

const uint8_t def_base64_decode_map[256] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 62, 255, 255, 255, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255,
255, 255, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255, 255, 26, 27, 28,
29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
49, 50, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

stBase64Setup::stBase64Setup(){
	strcpy_q(base64_map,sizeof(base64_map),def_base64_map);
	base64_mod=def_base64_mod;
	memcpy(base64_decode_map,def_base64_decode_map,sizeof(base64_decode_map));
}

void stBase64Setup::init_base64_map(char* pmap,char mod){
	if (strcmp(base64_map,pmap)!=0 && strchr(pmap,mod)==0){
		base64_mod=mod;
		strcpy_q(base64_map,sizeof(base64_map),pmap);
		memset(base64_decode_map,-1,sizeof(base64_decode_map));
		for (unsigned int i=0;i<sizeof(base64_map);i++){
			if (base64_map[i]!=0){
				base64_decode_map[(int)base64_map[i]]=(uint8_t)i;
			}
		}
	}
}

int stBase64Setup::base64_encode_size(int src_len)
{
	int base64_len = 4 * ((src_len+2)/3);
	return base64_len;
}

int stBase64Setup::base64_decode_size(char *src,int src_len)
{
	for (;src_len>0;src_len--){
		if (src[src_len-1]!=base64_mod){
			break;
		}
	}
	int decode_len = src_len * 3 / 4;
	return decode_len;
}

int stBase64Setup::base64_encode(char *src, int src_len, char *dst,int dst_len){
	int base64_len = 4 * ((src_len+2)/3);

	if (dst_len>=base64_len){
		int i = 0, j = 0;
		int nmod=src_len % 3;
		for (; i < src_len - nmod; i += 3){
			dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
			dst[j++] = base64_map[((src[i] << 4) | ((src[i + 1] >> 4) & 0x0f)) & 0x3f];
			dst[j++] = base64_map[((src[i + 1] << 2) | ((src[i + 2] >> 6 ) & 0x03 )) & 0x3f];
			dst[j++] = base64_map[src[i + 2] & 0x3f];
		}
		if (nmod == 1){
			dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
			dst[j++] = base64_map[(src[i] << 4) & 0x30];
			dst[j++] = base64_mod;
			dst[j++] = base64_mod;
		}else if (nmod == 2){
			dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
			dst[j++] = base64_map[((src[i] << 4) | ((src[i + 1] >> 4) & 0x0f)) & 0x3f];
			dst[j++] = base64_map[(src[i + 1] << 2) & 0x3c];
			dst[j++] = base64_mod;
		}
		if (dst_len>base64_len){ dst[j] = '\0';}
		return j;
	}
	return 0;
}

int stBase64Setup::base64_decode(char *src, int src_len, char *dst,int dst_len)
{
	for (;src_len>0;src_len--){
		if (src[src_len-1]!=base64_mod){
			break;
		}
	}
	int decode_len = src_len * 3 / 4;
	if (dst_len>=decode_len){
		int i = 0, j = 0;
		int nmod=src_len % 4;
		for (; i < src_len - nmod; i += 4) {
			dst[j++] = (base64_decode_map[(int)src[i]] << 2) |
				((base64_decode_map[(int)src[i + 1]] >> 4) & 0x03);
			dst[j++] = (base64_decode_map[(int)src[i + 1]] << 4) |
				((base64_decode_map[(int)src[i + 2]] >> 2) & 0x0f);
			dst[j++] = (base64_decode_map[(int)src[i + 2]] << 6) |
				(base64_decode_map[(int)src[i + 3]]);
		}
		if (nmod == 2) {
			dst[j++] = (base64_decode_map[(int)src[i]] << 2) |
				((base64_decode_map[(int)src[i + 1]] >> 4) & 0x03);
		}else if (nmod == 3){
			dst[j++] = (base64_decode_map[(int)src[i]] << 2) |
				((base64_decode_map[(int)src[i + 1]] >> 4) & 0x03);
			dst[j++] = (base64_decode_map[(int)src[i + 1]] << 4) |
				((base64_decode_map[(int)src[i + 2]] >> 2) & 0x0f);
		}
		if (dst_len>decode_len){ dst[j] = '\0';}
		return j;
	}
	return 0;
}
stBase64Setup g_base64;

int base64_encode(char *src, int src_len, char *dst,int dst_len){
	return g_base64.base64_encode(src,src_len,dst,dst_len);
}
int base64_decode(char *src, int src_len, char *dst,int dst_len){
	return g_base64.base64_decode(src,src_len,dst,dst_len);
}
int base64_encode_size(int src_len){
	return g_base64.base64_encode_size(src_len);
}
int base64_decode_size(char *src,int src_len){
	return g_base64.base64_decode_size(src,src_len);
}
