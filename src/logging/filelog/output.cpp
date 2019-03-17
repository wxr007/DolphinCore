#include "logging/filelog/output.h"
//#include "dump/DumpErrorBase.h"

COutput::COutput(bool boFileNameReadOnly)
{
	char stPath[MAX_PATH] = {0};
	//GetModuleFileNameA(g_hinstance, stPath, MAX_PATH - 1);
	char* p = strrchr(stPath, '.');

	if(p)
	{
		*p = '_';
		p++;
	}

	time_t ti = time(NULL);
	tm* t = localtime(&ti);
	char szTime[MAX_PATH];
	strftime(szTime, MAX_PATH, "%y%m%d%H%M%S ", t);
	memset(m_strFile, 0x00, sizeof(m_strFile));
	sprintf_q(m_strFile, sizeof(m_strFile) - 1, "%s_%s.log", stPath, szTime);
	m_boFileNameReadOnly = boFileNameReadOnly;
}

COutput::COutput(const char * pch, bool boFileNameReadOnly)
{
	assert(pch != NULL);
	memset(m_strFile, 0x00, sizeof(m_strFile));
	strcpy_q(m_strFile, sizeof(m_strFile), pch);
	m_boFileNameReadOnly = boFileNameReadOnly;
}

COutput::~COutput()
{
}

void COutput::SetFileName(const char * pch)
{
	if(pch && !m_boFileNameReadOnly)
	{
		memset(m_strFile, 0x00, sizeof(m_strFile));
		strcpy_q(m_strFile, sizeof(m_strFile), pch);
	}
}

char * COutput::GetFileName()
{
	return m_strFile;
}

#define  MAX_WRITE_NUM		1024*64

void COutput::WriteBuf(void *lpbuf, int buflen)
{
	assert(lpbuf != NULL);
	FILE* fp = NULL;

	try
	{
		fopen_q(&fp, m_strFile, "ab");

		if(fp)
		{
			while(buflen > 0)
			{
				if(buflen >= MAX_WRITE_NUM)
				{
					fwrite(lpbuf, MAX_WRITE_NUM, 1, fp);
					lpbuf = (void*)((int64_t)(lpbuf)+(int64_t)MAX_WRITE_NUM);//WARN
					buflen = buflen - MAX_WRITE_NUM;
				}
				else
				{
					fwrite(lpbuf, buflen, 1, fp);
					buflen = 0;
				}
			}

			fclose(fp);
			fp = NULL;
		}
	}
	catch(...)
	{
		if(fp)
		{
			fclose(fp);
			fp = NULL;
		}
	}
}
void COutput::WriteInt(int64_t OutInt)
{
	char out[128];
	sprintf_q(out, sizeof(out), "%llu \r\n", OutInt);
	WriteString(out);
}

void  COutput::WriteString(bool bNewLine, const char * lpFmt, ...)
{
	assert(lpFmt != NULL);
	char szText[2048];
	va_list argList;
	va_start(argList, lpFmt);
	vsprintf_q(szText, sizeof(szText), lpFmt, argList);
	va_end(argList);
	WriteString(szText, bNewLine);
}

void COutput::WriteString(const char * pstr, bool bNewLine)
{
	assert(pstr != NULL);
	FILE* fp = NULL;

	try
	{
		fopen_q(&fp, m_strFile, "ab");

		if(fp)
		{
			fputs(pstr, fp);

			if(bNewLine)
			{
				fputs("\r\n", fp);
			}

			fclose(fp);
			fp = NULL;
		}
	}
	catch(...)
	{
		if(fp)
		{
			fclose(fp);
			fp = NULL;
		}
	}
}
void COutput::NewLine()
{
	FILE* fp = NULL;

	try
	{
		fopen_q(&fp, m_strFile, "ab");

		if(fp)
		{
			fputs("\r\n", fp);
			fclose(fp);
			fp = NULL;
		}
	}
	catch(...)
	{
		if(fp)
		{
			fclose(fp);
			fp = NULL;
		}
	}
}
void  COutput::ShowMessageBox(const char * lpFmt, ...)
{
	assert(lpFmt != NULL);
	char szText[2048];
	va_list argList;
	va_start(argList, lpFmt);
	vsprintf_q(szText, sizeof(szText), lpFmt, argList);
	va_end(argList);
//	MessageBoxA(0, szText, "ShowMessageBox", 0);
}
void COutput::ShowMessageBox(int64_t OutInt)
{
	char out[128];
	sprintf_q(out, sizeof(out), "%"PRIu64, OutInt);
//	MessageBoxA(0, out, "ShowMessageBox", 0);
}
