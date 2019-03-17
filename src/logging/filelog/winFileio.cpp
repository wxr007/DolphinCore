#include "logging/filelog/winFileio.h"
#include <vector>
#include <time.h>
//#include <sys/utime.h>
#include "logging/filelog/output.h"
//#include "dump/DumpErrorBase.h"
/*************************************************************/



bool FileSystem::createPath(const char *srcfile)
{
	char filebuf[MAX_PATH];
	ZeroMemory(filebuf, sizeof(filebuf));
	strncpy_q(filebuf, srcfile, sizeof(filebuf) - 2);
	replaceFrontlashPath(filebuf);

	if(filebuf[strlen(filebuf)-1] != '/')
	{
		strcat(filebuf, "//");
	}

	const char* file = &filebuf[0];

	char pathbuf[MAX_PATH];

	const char *dir;

	ZeroMemory(pathbuf, sizeof(pathbuf));

	uint32_t pathLen = 0;

	bool ret = false;

	while((dir = strchr(file, '/')) != NULL)
	{
		//strncpy_q(pathbuf + pathLen, file, dir - file);
        memcpy(pathbuf + pathLen, file, dir - file);
		pathbuf[pathLen + dir-file] = 0;

		if(strlen((const char *)&pathbuf[pathLen]) == 0)
		{
			file = dir + 1;
			continue;
		}

		if(!((strcmp((const char *)&pathbuf[pathLen], ".") == 0) || (strcmp((const char *)&pathbuf[pathLen], "..") == 0)))
		{
			if(!FileSystem::IsDirExist(pathbuf))
			{
				ret = (CreateDirectory_Q(pathbuf, NULL) == true);

				if(!ret)
				{
					break;
				}
			}
		}

		pathLen += dir - file;
		pathbuf[pathLen++] = '/';
		file = dir + 1;
	}

	return ret;
}



