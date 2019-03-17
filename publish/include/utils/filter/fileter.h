#ifndef _FILETER_H__
#define _FILETER_H__

#include"utils/once_call.h"
#include<list>

class CFilter: public QCOOL::Singleton<CFilter>	
{
private:
	std::list<std::string> m_forbidWords;
	std::list<std::string> m_forbidWordsNPC;

public:
	CFilter();
	~CFilter(){};
public:
	bool init(const char* szFilterFile);			//初始化 fileter
	bool hasFilterStr(char *pText);					//是否过滤
	bool doFilter(char *pText, unsigned int len);	//过滤

	bool initNPC(const char* szFilterFile);			//初始化 NPC 名字fileter
	bool CheckNpcName(char *pText);					//检测NPCMING

};


#endif//_FILETER_H__
