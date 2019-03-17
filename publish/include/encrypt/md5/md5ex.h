#ifndef _MD5EX_H__
#define _MD5EX_H__

#pragma once
#include"platform/platform.h"
#include "encrypt/md5/md5.h"
#include<stddef.h>


bool MD5Data(const void* pData,size_t size,MD5_DIGEST *pMD5  );
bool MD5File(const char* pszFile,MD5_DIGEST *pMD5  );
bool MD5String(const char* string,MD5_DIGEST* pMD5  );

#ifdef _USE_ENGINE
	class Stream;
	bool MD5Stream(Stream* pStream,MD5_DIGEST *pMD5  );
#endif

#endif//_MD5EX_H__
