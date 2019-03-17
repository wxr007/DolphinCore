#include"utils/string/stringex.h"
#include"serialize/easyStrParse.h"



void replacelashPath(char *str, bool front)
{
//     char src = (!front) ? '/' : '/';
//     char dst = front ? '/' : '/';
	char src =  '/';
	char dst = '/';

    while(*str)
    {
        if(*str == src)
            *str = dst;

        str++;
    }
}


char* timetostr(time_t time1, char *szTime, int nLen, const char* sformat)
{
    struct tm tm1;
    memset(&tm1, 0, sizeof(tm1));
    _GET_TLS_LOOPCHARBUF(1024);
    localtime_q(&tm1, &time1);
    sprintf_q(ptlsbuf, ntlslen - 1, sformat, tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
    ptlsbuf[ntlslen-1] = 0;

    if(szTime)
    {
        strcpy_q(szTime, nLen, ptlsbuf);
    }

    return ptlsbuf;
}

char* find_path_dian_houzui(const char* pname)
{
    char* p = (char*)strrchr(pname, '.');

    if(p && p[1] != '\\' && p[1] != '/')
    {
        const char* p1 = safe_max(strrchr(pname, '/'), strrchr(pname, '/'));

        if(p > p1)
        {
            return p;
        }
    };

    return NULL;
}

const char* extractfilepath(const char* pname)
{
    if(!pname)
    {
        return pname;
    }

    _GET_TLS_LOOPCHARBUF(1024);
    strcpy_q(ptlsbuf, ntlslen - 1, pname);
    size_t nlen = strlen(ptlsbuf);

    if(nlen > 0)
    {
        char* p = find_path_dian_houzui(ptlsbuf);

        if(p == NULL)
        {
            if(ptlsbuf[nlen-1] != '/')
            {
                ptlsbuf[nlen] = '/';
                ptlsbuf[nlen+1] = 0;
            }
        }
        else
        {
            char* pend = ptlsbuf + nlen - 1;

            while(pend >= ptlsbuf)
            {
                if(*pend == '/')
                {
                    break;
                };

                pend--;
            };

            if(pend < ptlsbuf)
            {
                ptlsbuf[0] = 0;
            }
            else if(pend < p)
            {
                *(pend + 1) = 0;
            }
        }
    }

    ptlsbuf[ntlslen-1] = 0;
    return ptlsbuf;
}


const char* vformat(const char* pszFmt, ...)
{
    va_list ap;
    va_start(ap, pszFmt);
    _GET_TLS_LOOPCHARBUF(1024);
    vsprintf_q(ptlsbuf, ntlslen, pszFmt, ap);
    ptlsbuf[ntlslen-1] = 0;
    va_end(ap);
    return ptlsbuf;
}

void str_trim_right(char* psz)
{
    char* p = psz;
    char* p1 = NULL;

    while(*p)
    {
        if((*p) & 0x80)
        {
            if(iswspace(*((uint16_t*)p)))//uint16_t
            {
                if(p1 == NULL)
                    p1 = p;
            }
            else
            {
                if(p1)p1 = NULL;
            }

            p += 3;
        }
        else
        {
            if(isspace(*p))
            {
                if(p1 == NULL)
                    p1 = p;
            }
            else
            {
                if(p1)p1 = NULL;
            }

            p++;
        }
    }

    if(p1) *p1 = 0;
}

void str_trim_left(char* psz)
{
    char* p = psz;

    while(*p)
    {
        if((*p) & 0x80)
        {
            if(iswspace(*((uint16_t*)p))) {}//uint16_t
            else
            {
                break;
            }

            p += 3;
        }
        else
        {
            if(isspace(*p)) {}
            else
            {
                break;
            }

            p++;
        }
    }

    strcpy(psz, p);
}

void str_trim(char* psz)
{
    str_trim_left(psz);
    str_trim_right(psz);
}

int Mem2Hex(char* pin, int nsize, char* pout, int nout)
{
    for(int i = 0; i < nsize; i++)
    {
        sprintf_q(pout, nout - 1, "%.2x", uint8_t(pin[i]));
        pout += 2;
        nout -= 2;
    }

    return (nsize * 2);
}

int AbsMem2Hex(char* pin, int nsize, int nbef, int nmid, int naft, char* pout, int nout)
{
    if(nsize <= (nbef + nmid + naft))
    {
        return Mem2Hex(pin, nsize, pout, nout);
    }
    else
    {
        char* poldout = pout;
        int nret = Mem2Hex(pin, nbef, pout, nout);
        pout += nret;
        nout -= nret;
        strcat(pout, "...");
        pout += 3;
        nout -= 3;
        int nmidpos = (((nsize - nbef - naft - nmid) / 2) + nbef);
        nret = Mem2Hex(&pin[nmidpos], nmid, pout, nout);
        pout += nret;
        nout -= nret;
        strcat(pout, "...");
        pout += 3;
        nout -= 3;
        nret = Mem2Hex(&pin[nsize-naft], naft, pout, nout);
        pout += nret;
        //nout -= nret;
        *pout = 0;
        return (poldout - pout);
    }
}

char* endate2date(char* src)
{
    CEasyStrParse parse;
    char sztempdate[32];
    strncpy_q(sztempdate, src, sizeof(sztempdate) - 1);
    parse.SetParseStr(sztempdate, (char*)" ", NULL);
    src = parse[0];
    int mm = 0;

    switch(src[0])
    {
    case  'J':
    {
        if(src[1] == 'a')
            mm = 01;
        else if(src[2] == 'l')
            mm = 07;
        else
            mm = 06;
    }
    break;
    case 'F':
        mm = 02;
        break;
    case 'M':
    {
        if(src[2] == 'r')
            mm = 03;
        else
            mm = 05;
    }
    break;
    case 'A':
    {
        if(src[1] == 'u')
            mm = 04;
        else
            mm = 8;
    }
    break;
    case 'S':
        mm = 9;
        break;
    case 'O':
        mm = 10;
        break;
    case 'N':
        mm = 11;
        break;
    case 'D':
        mm = 12;
        break;
    }

    _GET_TLS_LOOPCHARBUF(1024);
    sprintf_q(ptlsbuf, ntlslen - 1, "%s-%2.2d-%s", parse[2], mm, parse[1]);
    ptlsbuf[ntlslen-1] = 0;
    return ptlsbuf;
}


const char* sec2str(int iSecond)
{
    int second = iSecond % 60;
    int minute = iSecond / 60;
    int hour =   minute / 60;
    int day =    hour / 24;
    _GET_TLS_LOOPCHARBUF(1024);
    if( iSecond <  0 )
    {
        return "";
    }
    else if( iSecond < 60 )
    {
        sprintf_q(ptlsbuf,ntlslen-1,"%d 秒",iSecond);
        ptlsbuf[ntlslen-1] = 0;
    }
    else if( iSecond < 60*60 )
    {
        sprintf_q(ptlsbuf,ntlslen-1,"%d分%d秒",minute%60,second);
        ptlsbuf[ntlslen-1] = 0;
    }
    else if( iSecond < 60*60*24 )
    {
        sprintf_q(ptlsbuf,ntlslen-1,"%d小时%d分%d秒",hour,minute%60,second);
        ptlsbuf[ntlslen-1] = 0;
    }
    else
    {
        sprintf_q(ptlsbuf,ntlslen-1,"%d天%d小时%d分%d秒",day,hour%24,minute%60,second);
        ptlsbuf[ntlslen-1] = 0;
    }
    return ptlsbuf;
}








size_t string_key_hash::operator()(const std::string &x) const
{
#ifdef _NOT_USE_STLPORT
    /*stdext::*/hash<const char *> H;
#else
    hash<const char *> H;
#endif
    return H(x.c_str());
}

size_t string_key_case_hash::operator()(const std::string &x) const
{
    ZSTACK_ALLOCA(char*,ptmpbuf,x.length()+1);
    strcpy(ptmpbuf,x.c_str());
    strlwr_q(ptmpbuf);
#ifdef _NOT_USE_STLPORT
    /*stdext::*/hash<const char *> H;
#else
    hash<const char *> H;
#endif
    return H(ptmpbuf);
}

bool string_key_equal::operator()(const std::string &s1, const std::string &s2) const
{
    return strcmp(s1.c_str(), s2.c_str()) == 0;
}

bool string_key_case_equal::operator()(const std::string &s1, const std::string &s2) const
{
    return stricmp_q(s1.c_str(), s2.c_str()) == 0;
}

size_t pchar_key_hash::operator()(const char* x) const
{
#ifdef _NOT_USE_STLPORT
    /*stdext::*/hash<const char *> H;
#else
    hash<const char *> H;
#endif
    return H(x);
}

size_t pchar_key_case_hash::operator()(const char* x) const
{
    ZSTACK_ALLOCA(char*,ptmpbuf,strlen(x)+1);
    strcpy(ptmpbuf,x);
    strlwr_q(ptmpbuf);
#ifdef _NOT_USE_STLPORT
    /*stdext::*/hash<const char *> H;
#else
    hash<const char *> H;
#endif
    return H(ptmpbuf);
}

bool pchar_key_equal::operator()(const char* s1, const char* s2) const
{
    return strcmp(s1, s2) == 0;
}

bool pchar_key_case_equal::operator()(const char* s1, const char* s2) const
{
    return stricmp_q(s1, s2) == 0;
}

//=========================================================================
const char* extractfiletitle(const char* pname)
{
    return changefileext(extractfilename(pname),"");
}

const char* extractfilename(const char* pname)
{
    if (!pname)
    {
        return pname;
    }

    const char* p=safe_max(strrchr(pname,'\\'),strrchr(pname,'/'));
    if (p)
    {
        p++;
    }
    else
    {
        p=pname;
    }

    _GET_TLS_LOOPCHARBUF(1024);
    strcpy_q(ptlsbuf,ntlslen-1,p);
    ptlsbuf[ntlslen-1] = 0;
    return ptlsbuf;
}

const char* changefileext(const char* pname,const char* pchg)
{
    if (!pname)
    {
        return pname;
    }

    _GET_TLS_LOOPCHARBUF(1024);

    strcpy_q(ptlsbuf,ntlslen-1,pname);

    char* p=find_path_dian_houzui(ptlsbuf);
    if (p && strlen(pchg)>0 && pchg[0]!='.')
    {
        p++;
    };
    if (p)
    {
        strcpy_q(p,ntlslen-(p-ptlsbuf)-1,pchg);
    }
    return ptlsbuf;
}


void deleOne(void*p,const char* szName,const char* szFile,int nLine,const char* szFuc)
{
//    map<void* ,stFuckTest>::iterator it;
//    it=g_mpNewData.find(p);
//    if (it!=g_mpNewData.end())
//    {
//        it->second.bfree=true;
//        sprintf_q(it->second.szDelName,500,"file:%s line:%d func:%s",szFile,nLine,szFuc);
//    }
}


void addOne(void*p,const char* szName,const char* szFile,int nLine,const char* szFuc)
{
//    stFuckTest stfuc;
//    stfuc.bfree=false;
//    stfuc.p=p;
//    sprintf_q(stfuc.szAddName,500,"name:%s file:%s line:%d func:%s",szName,szFile,nLine,szFuc);
//
//    g_mpNewData[p]=stfuc;
}




void showAll()
{

//        map<void* ,stFuckTest>::iterator it;
//        int ncount=0;
//
//        for (it=g_mpNewData.begin(); it!=g_mpNewData.end() ; it++ )
//        {
//            if (it->second.bfree==false)
//            {
//                g_logger.debug("memory lose %x add:%s",it->second.p,it->second.szAddName);
//                ncount++;
//            }
//        }
//
//        g_logger.debug("memory lose count%d",ncount);

}



void str_replace(const char* pszSrc, char* pszDst, int ndlen , const char* pszOld, const char* pszNew)
{
	const char* p, *q;
	char *q1;
	int len_old = (int)strlen(pszOld);
	int len_new = (int)strlen(pszNew);
	int nnext = (int)strlen(pszSrc) + (len_new - len_old);
	int l;
	q = pszSrc;
	q1 = pszDst;

	for(;;)
	{
		p = strstr(q, pszOld);

		if(p == NULL || nnext >= ndlen)
		{
			strcpy(q1, q);
			break;
		}
		else
		{
			l = (int)(p - q);
			memcpy(q1, q, l);
			q1 += l;
			memcpy(q1, pszNew, len_new);
			q1 += len_new;
			q = p + len_old;
			q1[0] = 0;
			nnext += (len_new - len_old);
		}
	}
}
void str_replace(char* pszSrc, const char* pszOld, const char* pszNew)
{
	int len = (int)strlen(pszSrc);
	STACK_ALLOCA(char*, psz, len * 4);

	if(psz)
	{
		str_replace(pszSrc, psz, len * 2, pszOld, pszNew);
		strcpy(pszSrc, psz);
	}
}
void str_replace(char* pszSrc, char cOld, char cNew)
{
	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			if(!(*(pszSrc + 1))) break;

			pszSrc += 3;
		}
		else
		{
			if(*pszSrc == cOld)
				*pszSrc = cNew;

			pszSrc++;
		}
	}
}

void str_replace(char* pszSrc, wchar_t cOld, wchar_t cNew)
{
	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			if(!(*(pszSrc + 1))) break;

			if((*(wchar_t*)pszSrc) == cOld)
			{
				(*(wchar_t*)pszSrc) = cNew;
			}

			pszSrc += 3;
		}
		else
		{
			pszSrc++;
		}
	}
}

std::string str_Replace(std::string &strText, const char *pszSrcText, const char *pszDestText)
{
	char *p = (char*)strstr(strText.c_str(), pszSrcText);

	if(!p) return strText;

	size_t size = (size_t)(p - strText.c_str());

	while(size <= strText.length() - strlen(pszSrcText) || !p)
	{
		strText.replace(size, strlen(pszSrcText), pszDestText);

		if(size_t (p - strText.c_str()) > strText.length() - 2) break;

		p = strstr(p + 1, pszSrcText);

		if(!p) break;

		size = (size_t)(p - strText.c_str()) + 1;
	}

	return strText;
}
