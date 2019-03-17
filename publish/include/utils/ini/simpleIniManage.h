#ifndef _SIMPLE_INI_MANAGE_H__
#define _SIMPLE_INI_MANAGE_H__

#include "utils/once_call.h"
#include"utils/ini/simpleIni.h"
#include"mem/synclist.h"

//因为是单线程的 所以这里为了效率并没有进行加锁的操作 !!!!
class CSimpleIniManage:public QCOOL::Singleton<CSimpleIniManage>
{
public:

	CSimpleIniManage();
	~CSimpleIniManage();
	void clearAllMemFile();
	CSimpleIni* getMemFileData(const char*szfilename,bool bforceupdate=false);
private:
	CSimpleIni* putMemFileData(const char*szfilename);
	void		MemFileForceUpdateData(CSimpleIni* iniold,const char*szfilename);
private:
	typedef CSyncMap<std::string,CSimpleIni*> tpMapFileMem;
	tpMapFileMem m_FileMemData;
};



#endif//_SIMPLE_INI_MANAGE_H__
