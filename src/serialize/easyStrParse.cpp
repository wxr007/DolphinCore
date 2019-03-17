#include"serialize/easyStrParse.h"
#include "define/common.h"

const char CEasyStrParse::NullChar[4] = {0, 0, 0, 0};

void CEasyStrParse::ParseParam(int nCount)
{
	if(m_pStart && m_pStart[0] != 0 && nCount > 0)
	{
		if(m_pch == NULL || m_pch[0] == 0)
		{
			if(m_pszParams.size() == 0)
			{
				m_pszParams.push_back(m_pStart);
				m_pStart = (char *)&NullChar;
			}

			return;
		}

		if((size_t)nCount <= m_pszParams.size())
		{
			return;
		}

		int b_a_idx = -1;
		int tmpidx = 0;
		int curch_state = 0;
		char* pread = m_pStart;
		char* pwrite = m_pStart;
		char* m_laststart = pwrite;

		while(*pread)
		{
			char curch = *pread;
			bool isfirstb_a = false;
			bool iszychar = false;

			switch(curch_state)
			{
			case 0:
				{
					tmpidx = 0;

					while(m_pBAch[tmpidx])
					{
						if(curch == m_pBAch[tmpidx])
						{
							isfirstb_a = true;
							b_a_idx = tmpidx + 1;
							curch_state = 1;
							break;
						}

						tmpidx += 2;
					}
				}
				break;
			case 1:
				{
					if(curch == m_zychr && (b_a_idx >= 0 && pread[1] != 0 && pread[1] == m_pBAch[b_a_idx]))
					{
						iszychar = true;
						pread++;
					}
					else if(b_a_idx >= 0 && curch == m_pBAch[b_a_idx])
					{
						curch_state = 0;
						b_a_idx = -1;
					}
				}
				break;
			}

			if(!iszychar && (isfirstb_a || curch_state == 0))
			{
				tmpidx = 0;

				while(m_pch[tmpidx])
				{
					if(curch == m_pch[tmpidx])
					{
						*pwrite = '\0';
						*pread = '\0';

						if(m_bokeepnil || m_laststart[0] != 0)
						{
							m_pszParams.push_back(m_laststart);
							m_laststart = pwrite;
							m_laststart++;

							if(m_pszParams.size() >= (size_t)nCount && curch_state == 0)
							{
								m_pStart = pread;
								m_pStart++;
								return;
							}
							else if(pread[1] == 0)
							{
								m_laststart = NULL;
								m_pStart = pread;
								m_pStart++;
								return;
							}
						}
						else
						{
							m_laststart = pwrite;
							m_laststart++;
						}

						break;
					}

					tmpidx++;
				}
			}

			*pwrite = *pread;
			pread++;
			pwrite++;
		}

		*pwrite = *pread;

		if(m_laststart)
		{
			if(m_laststart[0] != 0)
			{
				m_pszParams.push_back(m_laststart);
			}

			m_laststart = NULL;
		}

		m_pStart = pread;
	}

	return;
}

CEasyStrParse::CEasyStrParse()
{
	m_pStart = NULL;
	m_pSrcStart = NULL;
	m_pch = NULL;
	m_pBAch = NULL;
	m_psrc = NULL;
	m_zychr = 0;
	m_npsrc_maxlen = 0;
	m_bokeepnil = false;
}

CEasyStrParse::~CEasyStrParse()
{
	if(m_psrc)
	{
		__mt_char_alloc.deallocate(m_psrc);
	}

	m_psrc = NULL;
	m_npsrc_maxlen = 0;
}





bool CEasyStrParse::SetParseStrEx(const char* psz, char* pszCh, char* pszBAch, char zychr, bool keepnil)
{
	size_t nlen = (strlen(psz) + 16);

	if(m_psrc && m_npsrc_maxlen < nlen)
	{
		__mt_char_alloc.deallocate(m_psrc);
		m_psrc = NULL;
		m_npsrc_maxlen = 0;
	}

	if(m_psrc == NULL)
	{
		m_psrc = (char*)__mt_char_alloc.allocate(INT32(nlen));
	}

	if(m_psrc)
	{
		m_npsrc_maxlen = nlen;
		strcpy_q(m_psrc, nlen, psz);
		return SetParseStr(m_psrc, pszCh, pszBAch, zychr, keepnil);
	}

	return false;
}
bool CEasyStrParse::SetParseStr(char* psz, char* pszCh, char* pszBAch, char zychr, bool keepnil)
{
	m_pszParams.clear();
	m_pszParams.reserve(8);
	m_zychr = zychr;
	m_pStart = NULL;
	m_pSrcStart = NULL;
	m_pch = NULL;
	m_pBAch = NULL;
	m_bokeepnil = keepnil;

	if(SetBAch(pszBAch))
	{
		if(psz)
		{
			m_pSrcStart = psz;
			m_pStart = psz;
			SetCh(pszCh);
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool CEasyStrParse::SetCh(char* psz)
{
	m_pch = (char *)&NullChar;

	if(psz)
	{
		m_pch = psz;
		return true;
	}

	return false;
}

bool CEasyStrParse::SetBAch(char* psz)
{
	m_pBAch = (char *)&NullChar;

	if(psz == NULL)
	{
		return true;
	}
	else if((strlen(psz) % 2) == 0)
	{
		m_pBAch = psz;
		return true;
	}

	return false;
}


char* CEasyStrParse::Param(int i)
{
	char* pRet = NULL;
	i++;

	if(i < 1)
	{
		return m_pSrcStart;
	}

	if((size_t)i <= m_pszParams.size())
	{
		pRet = m_pszParams[i-1];
	}
	else
	{
		ParseParam(i);

		if((size_t)i <= m_pszParams.size())
		{
			pRet = m_pszParams[i-1];
		}
	}

	if(pRet == NULL)
	{
		pRet = (char *)&NullChar;
	}

	return pRet;
}
