#include"utils/ini/simpleIniManage.h"

//[ini]
int GetModuleFileName(char* sModuleName, char* sFileName, int nSize)
{
    std::string _exeName = "/proc/self/exe";

    size_t linksize = 256;
    char exeName[256] = { 0 };

    int ret = readlink(_exeName.c_str(), exeName, linksize);
    if (ret != -1)
    {
        char *tmp = strrchr(exeName, '/');

        if (tmp != NULL)
        {
            strcpy_q(sFileName, nSize, tmp + 1);
        }
    }
    return ret;
}

//[ini] 
void FilterFirSpaceAndEndTab(const char*szIn, char*szOut)
{
    const char*pstart = szIn;
    const char*pend = &szIn[strlen(szIn) - 1];
    int nlen = 0;
    while (*pstart == ' ' || *pstart == '\t')
    {
        pstart++;
    }
    while (*pend == ' ' || *pend == '\t')
    {
        pend--;
    }
    nlen = (int)(pend - pstart) + 1;
    if (nlen <= 0)
    {
        return;
    }
    memcpy(szOut, pstart, nlen);
    szOut[nlen] = 0;
}

////[ini] 屏蔽 只屏蔽一次 前后" 并且是配对的
void FileterFirAndEndQuotes(const char*szIn, char* szOut, uint32_t dwsize)
{
    if (szIn == NULL || dwsize < strlen(szIn))
    {
        return;
    }
    const char*szstart = szIn;
    const char*szend = &szIn[strlen(szIn) - 1];
    if ((*szstart) == '\"' && (*szend) == '\"'&&szstart != szend)
    {
        int nlen = (int)(szend - szstart) - 1;
        if (nlen <= 0)
        {
            return;
        }
        memcpy(szOut, szstart + 1, nlen);
        szOut[nlen] = 0;
    }
    else
    {
        strcpy(szOut, szIn);
    }
    return;
}
//[ini] 
uint32_t GetPrivateProfileStringA(char* ptzSectionName, char*  ptzKeyName, char*  ptzDefault, char*  ptzReturnedString, uint32_t dwSize,  char* szFilename, bool bforceupdate)
{

    int64_t dwtm1 = GetTickCount64_Q();
    CSimpleIni *ini = NULL;
    ini = CSimpleIniManage::getMe().getMemFileData(szFilename, bforceupdate);
    char *szsection = NULL;
    char *szkey = NULL;
    int nret = 0;
    if (ini == NULL)
    {
        return nret;
    }
    if (ptzSectionName)
    {
        int nsection = strlen(ptzSectionName) + 1;
        szsection = new char[nsection];
        FilterFirSpaceAndEndTab(ptzSectionName, szsection);//过滤key的名字
    }
    if (ptzKeyName)
    {
        int nkey = strlen(ptzKeyName) + 1;
        szkey = new char[nkey];
        FilterFirSpaceAndEndTab(ptzKeyName, szkey);//过滤key的名字
        const char* p = ini->GetValue(szsection, szkey, ptzDefault);
        FileterFirAndEndQuotes(p, ptzReturnedString, dwSize);//过滤"
        //strcpy_s(ptzReturnedString,dwSize,p);
        nret = p == NULL ? 0 : strlen(ptzReturnedString);//windows下 这里不包含\0
    }
    else
    {
        CSimpleIniA::TNamesDepend keys;
        ini->GetAllKeys(szsection, keys);
        CSimpleIniA::TNamesDepend::iterator itb;
        char*pinbuf = ptzReturnedString;
        int nmaxlen = dwSize - 1;//内存最大数量
        for (itb = keys.begin(); itb != keys.end(); ++itb)
        {
            int stlen = strlen(itb->pItem) + 1;//包含 '\0'
            if (stlen < nmaxlen)
            {
                memcpy(pinbuf, itb->pItem, stlen);
                pinbuf += stlen;
                nmaxlen = nmaxlen - stlen;
            }
            else
            {
                //g_logger.error("reasection all keys memory not  enough");
                break;
            }
        }
        *pinbuf = '\0';
        nret = dwSize - nmaxlen;//windows下 这里就是所有字节数
    }

    SAFE_DELETE_VEC(szsection);
    SAFE_DELETE_VEC(szkey);

    int64_t dwtm2 = GetTickCount64_Q();
    if (dwtm2 - dwtm1 > 20)
    {
        //这里调用了 thread buf 所以不能使用
        //g_logger.forceLog(zLogger::zERROR,"GetPrivateProfileStringA time %d ",dwtm2-dwtm1);
    }
    return nret;
}
//[ini]
bool WritePrivateProfileStringA(char* ptzSectionName, char* ptzKeyName, char* szdata, char* szfilename)
{
    int64_t dwtm1 = GetTickCount64_Q();
    CSimpleIni *ini = CSimpleIniManage::getMe().getMemFileData(szfilename);
    bool bret = false;
    if (ini == NULL)
    {
        return bret;
    }
    if (ptzKeyName == NULL&&ptzSectionName != NULL)
    {

        bret = ini->Delete(ptzSectionName, NULL);
    }

    if (szdata == NULL&&ptzKeyName != NULL&&ptzSectionName != NULL)
    {
        bret = ini->Delete(ptzSectionName, ptzKeyName);
    }

    if (ptzSectionName != NULL&&ptzKeyName != NULL&&szdata != NULL)
    {
        SI_Error nret = ini->SetValue(ptzSectionName, ptzKeyName, szdata);
        bret = nret<0 ? false : true;
    }

    ini->SaveFile(szfilename);
    int64_t dwtm2 = GetTickCount64_Q();
    if (dwtm2 - dwtm1>20)
    {
        //这里调用了 thread buf 所以不能使用
        //g_logger.forceLog(zLogger::zERROR,"GetPrivateProfileStringA time %d ",dwtm2-dwtm1);
    }
    return bret;
}


CSimpleIniManage::CSimpleIniManage()
{
	m_FileMemData.clear();
}
CSimpleIniManage::~CSimpleIniManage()
{
    clearAllMemFile();
}
void CSimpleIniManage::clearAllMemFile()
{

	tpMapFileMem::iterator itfir,itnext;
	for (itnext=m_FileMemData.begin(),itfir=itnext;itfir!=m_FileMemData.end();itfir=itnext)
	{
		++itnext;
		CSimpleIni*simini=itfir->second;
		if (simini)
		{
			SAFE_DELETE(simini);
		}


	}
	m_FileMemData.clear();
}

CSimpleIni* CSimpleIniManage::putMemFileData(const char*szfilename)
{
		if(szfilename==NULL)
		{
			return NULL;
		}

		CSimpleIni* simini=new CSimpleIni(false,false,true);//gbk模式 非utf-8 模式
		simini->LoadFile(szfilename);
		m_FileMemData[szfilename]=simini;
		return simini;
}

//强制更新 ini的数据
void CSimpleIniManage::MemFileForceUpdateData(CSimpleIni* iniold,const char*szfilename)
{
	if(iniold==NULL||szfilename==NULL)
	{
		return;
	}
	iniold->Reset();
	iniold->LoadFile(szfilename);
}

CSimpleIni* CSimpleIniManage::getMemFileData(const char*szfilename,bool bforceupdate)
{
		if (szfilename==NULL)
		{
			return NULL;
		}

		tpMapFileMem::iterator it;
		it=m_FileMemData.find(szfilename);
		if (it!=m_FileMemData.end())
		{
			if (bforceupdate==true)
			{
				//强制更新ini数据
				MemFileForceUpdateData(it->second,szfilename);
			}
			return it->second;
		}
		else
		{
			return putMemFileData(szfilename);
		}

}
