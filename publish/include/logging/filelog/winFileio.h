#ifndef _WIN_FILE_IO_H__
#define _WIN_FILE_IO_H__

#include"define/define.h"
#include "platform/platform.h"
#include <vector>
#include "utils/string/stringex.h"


namespace FileSystem
{
bool delfile(const char* fileName = NULL);
bool createPath(const char *srcfile);
bool replacefile(const char* srcFileName, const char* dstFileName, bool delsrc = true);
static __inline bool IsDirExist(const char* pszDir)
{
	return IsDirOrFileExist(pszDir);
}
static __inline bool IsFileExist(const char* pszFileName)
{
	return IsDirOrFileExist(pszFileName);
}
};



#endif // _WIN_FILE_IO_H__
