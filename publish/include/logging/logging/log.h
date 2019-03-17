
#ifndef _LOG_H__
#define _LOG_H__

#ifndef _TEST_SELF_//如果not define ,server1.5的logger.
#include"logging/logging/zLogger.h"

extern zLogger g_logger;

#define LOGERROR(fmt,...) g_logger.error(fmt,##__VA_ARGS__)
#define LOGWARN(fmt,...)  g_logger.warn(fmt,##__VA_ARGS__)
#define LOGDEBUG(fmt,...) g_logger.debug(fmt,##__VA_ARGS__)
#define LOGINFO(fmt,...)  g_logger.info(fmt,##__VA_ARGS__)

#else//仅仅是为了测试用的.

#include <cstdio>
extern FILE *fplog;
#define LOGERROR(fmt,...) do{fprintf(fplog,"  ERROR |");fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0);
#define LOGWARN(fmt,...) do{ fprintf(fplog,"LOGWARN |");fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0);
#define LOGDEBUG(fmt,...) do{fprintf(fplog,"LOGDEBUG|");fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0);
#define LOGINFO(fmt,...) do{ fprintf(fplog,"LOGINFO |");fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0);

// #define LOGERROR(fmt,...) do{printf(fmt,##__VA_ARGS__);printf("\n");}while(0);
// #define LOGWARN(fmt,...) do{printf(fmt,##__VA_ARGS__);printf("\n");}while(0);
// #define LOGDEBUG(fmt,...) do{printf(fmt,##__VA_ARGS__);printf("\n");}while(0);
// #define LOGINFO(fmt,...) do{printf(fmt,##__VA_ARGS__);printf("\n");}while(0);
#endif

#endif //_LOG_H__
