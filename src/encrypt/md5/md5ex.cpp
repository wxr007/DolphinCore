#include "encrypt/md5/md5ex.h"
#include <stdio.h>
#include <string.h>
#include "define/common.h"


bool MD5Data(const void* pData,size_t size,MD5_DIGEST *pMD5  )
{
	MD5_CTX context;
	MD5Init (&context);
	MD5Update (&context, (unsigned char*)pData, UINT32(size));
	MD5Final (&context,pMD5);
	return true;
}

#ifdef _USE_ENGINE

bool MD5Stream(Stream* pStream,MD5_DIGEST *pMD5  )
{
	MD5_CTX context;
	unsigned char buffer[4096];
	size_t size = pStream->getSize();
	MD5Init (&context);
	while(size)
	{
		size_t readSize = getMin(size,(size_t)4096);
		if(!pStream->read(buffer,readSize))
		{
			MD5Final (&context,pMD5);
			return false;
		}
		MD5Update (&context, buffer, readSize);
	}
	MD5Final (&context,pMD5);
	return true;
}

bool MD5File(const char* pszFile,MD5_DIGEST *pMD5  )
{
	FileStream stream;
	if(!stream.open(pszFile,FileStream::Read))
		return false;
	return MD5Stream(&stream,pMD5);
}

#else

bool MD5File(const char* pszFile,MD5_DIGEST *pMD5  )
{
	FILE* fp =0;
	fopen_q(&fp,pszFile,"rb");
	if(!fp) return false;

	unsigned char buffer[8192];
	size_t len;
	fseek(fp,0,SEEK_END);
	len = ftell(fp);
	fseek(fp,0,SEEK_SET);

	MD5_CTX context;
	MD5Init (&context);
	while( len ){
		size_t readLen = (len < 8192 ? len : 8192);
		if(1 != fread(buffer,readLen,1,fp)){
			MD5Final (&context,pMD5);
			fclose(fp);
			fp=NULL;
			return false;
		}
		len -= readLen;
		MD5Update (&context, buffer, UINT32(readLen));
	}
	MD5Final (&context,pMD5);
	if (fp){fclose(fp);fp=NULL;}
	return true;
}

#endif

bool MD5String(const char* string,MD5_DIGEST* pMD5  )
{
	size_t size = strlen(string);
	return MD5Data(string,size,pMD5);
}
