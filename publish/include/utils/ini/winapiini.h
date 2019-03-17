#ifndef _WINAPIINI_H__
#define _WINAPIINI_H__

#pragma once
#include "define/define.h"
#include <stdint.h>

//#include "dump/DumpErrorBase.h"



class CWinApiIni
{
public:

	char m_szFileName[MAX_PATH];
	char m_szOpSecName[MAX_PATH];
public:

	__inline CWinApiIni()
	{
	    GetModuleFileName_Q(NULL, m_szFileName, MAX_PATH);
		//GetModuleFileNameA(g_hinstance, m_szFileName, MAX_PATH);
		char* p = strrchr(m_szFileName,'.');
		if(p){*p='_';p++;}
		strcat(m_szFileName,".ini");
	}


	__inline CWinApiIni(char* ptzFileName)
	{
		strcpy_q(m_szFileName,MAX_PATH, ptzFileName);
	}


	__inline void SetOpSection(char* ptzSectionName)
	{
		strcpy_q(m_szOpSecName,MAX_PATH, ptzSectionName);
	}

	__inline void SetOpFile(char* ptzFileName)
	{
		strcpy_q(m_szFileName,MAX_PATH, ptzFileName);
	}

	__inline uint32_t ReadInt(char* ptzSectionName, char* ptzKeyName, int32_t iDefault = 0)
	{
		char  szintbuf[256]={0};
		char szCh[] = {""};
		int n=GetPrivateProfileStringA(ptzSectionName, ptzKeyName,szCh,szintbuf,256 , m_szFileName);
		if(n<=0)
		{
			return 0;
		}
		else
		{
			return atoi(szintbuf);
		}
	}


	__inline bool WriteInt(char* ptzSectionName, char* ptzKeyName, int32_t iValue = 0)
	{
		char tzString[30];
		sprintf(tzString, "%d", iValue);
		return WritePrivateProfileStringA(ptzSectionName, ptzKeyName, tzString, m_szFileName);
	}


	__inline uint32_t ReadString(char* ptzSectionName, char* ptzKeyName, char* ptzReturnedString, uint32_t dwSize, char* ptzDefault,bool bforceupdate=false)
	{
			return GetPrivateProfileStringA(ptzSectionName, ptzKeyName, ptzDefault, ptzReturnedString, dwSize, m_szFileName,bforceupdate);
	}


	__inline bool WriteString(char* ptzSectionName, char* ptzKeyName,char* ptzString)
	{
		return WritePrivateProfileStringA(ptzSectionName, ptzKeyName, ptzString,m_szFileName);
	}


	__inline int64_t ReadInt64(char* ptzSectionName, char* ptzKeyName, int64_t iDefault)
	{
		char tzString[64]={0};
		char szCh[] = {""};
		ReadString(ptzSectionName,ptzKeyName,tzString,sizeof(tzString)-1,szCh);
		return strto64_q(tzString,NULL,10);
	}


	__inline bool WriteInt64(char* ptzSectionName, char* ptzKeyName, int64_t iValue)
	{
		char tzString[64]={0};
		sprintf_q(tzString,sizeof(tzString)-1,"%lld",iValue);
		return WriteString(ptzSectionName, ptzKeyName, tzString);
	}


	__inline bool ReadStruct(char* ptzSectionName, char* ptzKeyName, void* pvStruct, char* uSize)
	{
		return 1;//GetPrivateProfileStructA(ptzSectionName, ptzKeyName, pvStruct, uSize, m_szFileName);
	}


	__inline bool WriteStruct(char* ptzSectionName, char* ptzKeyName, void* pvStruct, uint32_t uSize)
	{
		return 1;//WritePrivateProfileStructA(ptzSectionName, ptzKeyName, pvStruct, uSize, m_szFileName);
	}


	__inline uint32_t ReadSection(char* ptzSectionName, char* ptzReturnBuffer, uint32_t dwSize)
	{
		return 1;//GetPrivateProfileSectionA(ptzSectionName, ptzReturnBuffer, dwSize, m_szFileName);
	}


	__inline uint32_t WriteSection(char* ptzSectionName, char* ptzString)
	{
		return 1;//WritePrivateProfileSectionA(ptzSectionName, ptzString, m_szFileName);
	}


	__inline uint32_t ReadSectionNames(char* ptzReturnBuffer, uint32_t dwSize)
	{
		return 1;//GetPrivateProfileSectionNamesA(ptzReturnBuffer, dwSize, m_szFileName);
	}






	__inline uint32_t ReadInt(char* ptzKeyName, int32_t iDefault = 0)
	{
		return ReadInt(m_szOpSecName,ptzKeyName,iDefault);
		//return 1;//GetPrivateProfileIntA(m_szOpSecName, ptzKeyName, iDefault, m_szFileName);
	}


	__inline bool WriteInt(char* ptzKeyName, int32_t iValue = 0)
	{
		char tzString[30];
		sprintf(tzString, "%d", iValue);
		return WritePrivateProfileStringA(m_szOpSecName, ptzKeyName, tzString, m_szFileName);
	}


	__inline uint32_t ReadString(char* ptzKeyName, char* ptzReturnedString, uint32_t dwSize, char* ptzDefault)
	{
		return GetPrivateProfileStringA(m_szOpSecName, ptzKeyName, ptzDefault, ptzReturnedString, dwSize, m_szFileName);
	}


	__inline bool WriteString(char* ptzKeyName, char* ptzString)
	{
		return WritePrivateProfileStringA(m_szOpSecName, ptzKeyName, ptzString, m_szFileName);
	}


	__inline int64_t ReadInt64(char* ptzKeyName, int64_t iDefault)
	{
		char tzString[64]={0};
		char szCh[] = {""};
		ReadString(ptzKeyName,tzString,sizeof(tzString)-1,szCh);
		return strto64_q(tzString,NULL,10);
	}


	__inline bool WriteInt64(char* ptzKeyName, int64_t iValue)
	{
		char tzString[64]={0};
		sprintf_q(tzString,sizeof(tzString)-1,"%lld",iValue);
		return WriteString(ptzKeyName, tzString);
	}


	__inline bool ReadStruct(char* ptzKeyName, void* pvStruct, uint32_t uSize)
	{
		return 1;//GetPrivateProfileStructA(m_szOpSecName, ptzKeyName, pvStruct, uSize, m_szFileName);
	}


	__inline bool WriteStruct(char* ptzKeyName, void* pvStruct, uint32_t uSize)
	{
		return 1;//WritePrivateProfileStructA(m_szOpSecName, ptzKeyName, pvStruct, uSize, m_szFileName);
	}


	__inline uint32_t ReadSection(char* ptzReturnBuffer, uint32_t dwSize)
	{
		return 1;//GetPrivateProfileSectionA(m_szOpSecName, ptzReturnBuffer, dwSize, m_szFileName);
	}


	__inline uint32_t WriteSection(char* ptzString)
	{
		return 1;//WritePrivateProfileSectionA(m_szOpSecName, ptzString, m_szFileName);
	}
};
#endif//end _WINAPIINI_H__
