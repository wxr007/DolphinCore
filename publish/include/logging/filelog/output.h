#ifndef _OUTPUT_H__
#define _OUTPUT_H__

#include "define/define.h"
#include <stdarg.h>
#include <assert.h>

#include <stdio.h>
#include<time.h>
#include <stdint.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "define/noncopyable.h"
#include "define/common.h"

class COutput
{
private:
	char m_strFile[MAX_PATH - 1];
	bool m_boFileNameReadOnly;
public:
	COutput(bool boFileNameReadOnly = false);
	COutput(const char * pch, bool boFileNameReadOnly = false);
	virtual ~COutput();

	void SetFileName(const char * pch);
	char * GetFileName();
	void WriteBuf(void *lpbuf, int buflen);
	void WriteInt(int64_t OutInt);
	void WriteString(bool bNewLine, const char * lpFmt, ...);
	void WriteString(const char * pstr, bool bNewLine = false);
	void NewLine();


	static void ShowMessageBox(const char * lpFmt, ...);
	static void ShowMessageBox(int64_t OutInt);

	static __inline void  OutDebugMsg(const char * lpFmt, ...)
	{
#ifdef _DEBUG
		assert(lpFmt != NULL);
		char szText[2048];
		va_list argList;
		va_start(argList, lpFmt);
		vsprintf_q(szText, sizeof(szText), lpFmt, argList);
		va_end(argList);
		_RPT1(_CRT_WARN, "%s\n", szText);
#else
		assert(lpFmt != NULL);
		char szText[2048];
		va_list argList;
		va_start(argList, lpFmt);
		//TODO : it si dargen
		vsprintf_q(szText, sizeof(szText), lpFmt, argList);
		va_end(argList);
		strcat(szText, "\n");
		//::OutputDebugString(szText);
#endif
	}
	static __inline void OutDebugMsg(int64_t OutInt)
	{
#ifdef _DEBUG
		_RPT1(_CRT_WARN, "%"PRIu64"\n", OutInt);
#else
		char szText[128];
		sprintf_q(szText, sizeof(szText), "%"PRIu64"\n", OutInt);
		//::OutputDebugString(szText);
#endif
	}
};

extern COutput	g_appout;




#endif // _OUTPUT_H__
