
#include"define/define.h"
#include <fstream>
#include"utils/filter/fileter.h"
#include"utils/convert_code/convEncode.h"
#include"logging/logging/zLogger.h"

#define _MAX_FILTER_SIZE_				1024*4
CFilter::CFilter()
{
}

bool CFilter::init(const char* szFilterFile)
{
	std::ifstream FilterSrc(szFilterFile);

	if(!FilterSrc)
	{
		g_logger.forceLog(zLogger::zERROR, "初始化名称过滤器失败,没有找到 %s 文件", szFilterFile);
		return false;
	}

	m_forbidWords.clear();
	char buf[_MAX_FILTER_SIZE_] = {0};
	ZeroMemory(buf, sizeof(buf));
	

	while(FilterSrc.getline(buf, sizeof(buf)))
	{
		if(buf[0] != 0)
		{
			size_t nlen=strlen(buf);
			if (buf[nlen-1] == '\r')//windows 和linux 文件系统换行不一致造成读取数据不一致 
			{
				buf[nlen-1]=0;
			}
			//传入字符串已经为UTF-8不需要在进行宽字节转换
			std::string TempText(buf);
			m_forbidWords.push_back(TempText);
		}
	}

	g_logger.forceLog(zLogger::zTRACE, "初始化名称过滤器成功");
	return true;
}

bool CFilter::initNPC(const char* szFilterFile)
{
	std::ifstream FilterSrc(szFilterFile);

	if (!FilterSrc)
	{
		g_logger.forceLog(zLogger::zERROR, "初始化名称过滤器失败,没有找到 %s 文件", szFilterFile);
		return false;
	}

	m_forbidWordsNPC.clear();
	char buf[_MAX_FILTER_SIZE_] = { 0 };
	ZeroMemory(buf, sizeof(buf));

	while (FilterSrc.getline(buf, sizeof(buf)))
	{
		if (buf[0] != 0)
		{
			size_t nlen = strlen(buf);
			if (buf[nlen - 1] == '\r')//windows 和linux 文件系统换行不一致造成读取数据不一致 
			{
				buf[nlen - 1] = 0;
			}
			std::string tempbuf(buf);
			m_forbidWordsNPC.push_back(tempbuf);
		}
	}
	g_logger.forceLog(zLogger::zTRACE, "初始化npc名称过滤器成功");
	return true;
}

bool CFilter::hasFilterStr(char *pText)
{
	if(!pText) return false;

	if(strlen(pText) >= (_MAX_FILTER_SIZE_ >> 1))
	{
		return true;
	}

	if (strlen(pText) > 0)
	{
		std::string TempText(pText);

		for(std::list<std::string>::const_iterator itr = m_forbidWords.begin(); itr != m_forbidWords.end(); ++itr)
		{
			std::string TempStr = *itr;

			if(TempStr.length() <= TempText.length() && TempStr.length() > 0)
			{
				std::string::size_type nPos = TempText.find(TempStr.c_str(), 0);

				if(nPos != std::string::npos)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CFilter::doFilter(char *pText, unsigned int len)
{
	if(!pText) return false;
	if (len>=(_MAX_FILTER_SIZE_>>1)){ len=((_MAX_FILTER_SIZE_>>1)-1);pText[len-1]=0; }
	//CodeTips:字符串编码为utf-8 去除wstring
	std::string TempText(pText);
	static std::string strdst("*");

	bool bFind =false;
	for(std::list<std::string>::const_iterator itr = m_forbidWords.begin();itr!=m_forbidWords.end();++itr){
		std::string TempStr = *itr;
		if(TempStr.length()<=TempText.length() && TempStr.length()>0 ){

			std::string::size_type pos = 0;

			while( (pos = TempText.find(TempStr, pos)) != std::string::npos)
            {
				TempText.replace(pos, TempStr.length(), strdst);
				pos += strdst.length();
				bFind = true;
			}
		}
	}
	if(bFind)
	{
		ZeroMemory(pText,len);
 //		WideCharToMultiByte((wchar_t*)TempText.c_str(),pText,len);//zc:note 这个应该是utf-8转gbk 应该没有用了 如果需要再调用UTF8ToGBK转换
	}
	return true;
}


bool CFilter::CheckNpcName(char *pText)
{

	if (!pText) return false;

	if (strlen(pText) >= (_MAX_FILTER_SIZE_ >> 1))
	{
		return true;
	}

	if (strlen(pText) > 0)
	{
		std::string TempText(pText);

		for (std::list<std::string>::const_iterator itr = m_forbidWordsNPC.begin(); itr != m_forbidWordsNPC.end(); ++itr)
		{
			std::string TempStr = *itr;

			if (TempStr.length() <= TempText.length() && TempStr.length() > 0)
			{
				std::string::size_type nPos = TempText.find(TempStr.c_str(), 0);

				if (nPos != std::string::npos)
				{
					return true;
				}
			}
		}
	}

	return false;
}
