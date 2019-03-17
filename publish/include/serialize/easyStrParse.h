#ifndef _EASYSTRPARSE_H__
#define _EASYSTRPARSE_H__

#include"platform/platform.h"
#include<vector>
#include"mem/lookaside_alloc.h"


class CEasyStrParse
{
protected:




	std::vector< char* >  m_pszParams;



	char* m_pStart;
	char* m_pSrcStart;

	char* m_pch;
	char* m_pBAch;

	char* m_psrc;
	size_t m_npsrc_maxlen;

	bool m_bokeepnil;


	char m_zychr;

	static const char NullChar[4];




	bool SetCh(char* psz);
	bool SetBAch(char* psz);

	void ParseParam(int nCount);

	char* Param(int i);
public:
	CEasyStrParse();
	virtual ~CEasyStrParse();





	bool SetParseStrEx(const char* psz, char* pszCh, char* pszBAch = NULL, char zychr = '\0', bool keepnil = false);
	bool SetParseStr(char* psz, char* pszCh, char* pszBAch = NULL, char zychr = '\0', bool keepnil = false);
	int ParamCount()
	{
		Param(0x7ffffff0);
		return (int)m_pszParams.size();
	}
	char * operator [](int  num)
	{
		return Param(num);
	}
};



#endif //_EASYSTRPARSE_H__
