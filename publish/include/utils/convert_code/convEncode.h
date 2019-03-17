#ifndef _CONV_ENCODE_H__
#define _CONV_ENCODE_H__

int GBKToUTF8(char * lpGBKStr,char * lpUTF8Str,int nUTF8StrLen);
int UTF8ToGBK(char * lpUTF8Str,char * lpGBKStr,int nGBKStrLen);

#endif //_CONV_ENCODE_H__