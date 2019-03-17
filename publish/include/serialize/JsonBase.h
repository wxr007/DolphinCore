#pragma once
#include <string>
#include <string.h>
#include <stdint.h>
#include <map>
#include <vector>
#include "platform/platform.h"
#include "jsoncpp/json/json.h"

namespace JSONTRANS
{
	inline void Data2Json(Json::Value& elem,const std::string& name,const bool& data)
	{
		elem[name] = data;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,const uint8_t& data)
	{
		elem[name] = data;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,const int16_t& data)
	{
		elem[name] = data;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,const uint16_t& data)
	{
		elem[name] = data;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,const int32_t& data)
	{
		elem[name] = data;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,const uint32_t& data)
	{
		elem[name] = data;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,const int64_t& data)
	{
		char szvalue[128];
		elem[name] = ui64toa_q(data,szvalue,10);
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,const uint64_t& data)
	{
		char szvalue[128];
		elem[name] = ui64toa_q(data,szvalue,10);
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,const std::string& data)
	{
		elem[name] = data;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,Json::Value& elem2)
	{
		elem[name] = elem2;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,char data[],size_t size)
	{
		elem[name] = data;
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,uint8_t data[], uint32_t size)
	{
		elem[name].clear();
		for (uint32_t i = 0;i < size; i++)
		{
			elem[name].append(data[i]);
		}
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,bool data[], uint32_t size)
	{
		elem[name].clear();
		for (uint32_t i = 0;i < size; i++)
		{
			elem[name].append(data[i]);
		}
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,int16_t data[], uint32_t size)
	{
		elem[name].clear();
		for (uint32_t i = 0;i < size; i++)
		{
			elem[name].append(data[i]);
		}
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,uint16_t data[], uint32_t size)
	{
		elem[name].clear();
		for (uint32_t i = 0;i < size; i++)
		{
			elem[name].append(data[i]);
		}
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,int data[], uint32_t size)
	{
		elem[name].clear();
		for (uint32_t i = 0;i < size; i++)
		{
			elem[name].append(data[i]);
		}
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,uint32_t data[], uint32_t size)
	{
		elem[name].clear();
		for (uint32_t i = 0;i < size; i++)
		{
			elem[name].append(data[i]);
		}
	}
	inline void Data2Json(Json::Value& elem,const std::string& name,int64_t data[], uint32_t size)
	{
		elem[name].clear();
		char szvalue[128];
		for (uint32_t i = 0;i < size; i++)
		{
			elem[name].append(ui64toa_q(data[i],szvalue,10));
		}
	}
	inline void Data2Json(Json::Value& elem, const std::string& name, const std::vector<int>& _v)
	{
		elem[name].clear();
		std::vector<int>::const_iterator it;
		int i = 0;
		for (it = _v.begin(); it != _v.end(); ++it, ++i)
		{
			elem[name][i] = *it;
		}
	}
	// 	// 这个用的蛮多的，写一个吧
	// 	inline void Data2Json(Json::Value& elem, const std::string& name, const std::vector<stAwardItem>& _v)
	// 	{
	// 		elem[name].clear();
	// 		std::vector<stAwardItem>::const_iterator it;
	// 		Json::Value tmp;
	// 		for (it = _v.begin(); it != _v.end(); ++it)
	// 		{
	// 			const stAwardItem& item = *it;
	// 			tmp["id"] = item.nItemId;
	// 			tmp["count"] = item.nCount;
	// 			elem[name].append(tmp);
	// 		}
	// 	}

	inline void Data2Json(Json::Value& elem, const std::string& name, const std::map<int, int>& _m)
	{
		elem[name].clear();
		Json::Value tmp;
		std::map<int,int>::const_iterator it;
		for (it = _m.begin(); it != _m.end(); ++it)
		{
			tmp["key"] = it->first;
			tmp["value"] = it->second;
			elem[name].append(tmp);
		}
	}

	//Load_Member========================================================
	inline void Json2Data(Json::Value& elem,const std::string& name,bool& data)
	{
		data = elem[name].asBool();
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,uint8_t& data)
	{
		data = elem[name].asUInt();
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,int16_t& data)
	{
		data = elem[name].asInt();
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,uint16_t& data)
	{
		data = elem[name].asInt();
	}

	inline void Json2Data(Json::Value& elem,const std::string& name,int& data)
	{
		data = elem[name].asInt();
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,uint32_t& data)
	{
		data = elem[name].asUInt();
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,int64_t& data)
	{
		if (elem.isMember(name))
		{
			if (elem[name].isString())
			{
				data = strto64_q( elem[name].asCString(), NULL, 10 );
			}
			else
			{
				data = elem[name].asInt( );
			}
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,uint64_t& data)
	{
		if (elem.isMember(name))
		{
			if (elem[name].isString())
			{
				data = strtou64_q( elem[name].asCString(), NULL, 10 );
			}
			else
			{
				data = elem[name].asInt( );
			}
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,std::string& data)
	{
		if (elem.isMember(name))
		{
			data = elem[name].asString();
		}
	}

	inline void Json2Data(Json::Value& elem,const std::string& name,char data[],size_t size)
	{
		if (elem.isMember(name))
		{
			if (strlen(elem[name].asCString()) < size)
			{
				strcpy(data,elem[name].asCString());
			}
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,bool data[], uint32_t size)
	{
		for (uint32_t i = 0;i < elem[name].size() && i < size;i++)
		{
			data[i] = elem[name][i].asBool();
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,uint8_t data[], uint32_t size)
	{
		for (uint32_t i = 0;i < elem[name].size() && i < size;i++)
		{
			data[i] = elem[name][i].asUInt();
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,int16_t data[], uint32_t size)
	{
		for (uint32_t i = 0;i < elem[name].size() && i < size;i++)
		{
			data[i] = elem[name][i].asInt();
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,uint16_t data[], uint32_t size)
	{
		for (uint32_t i = 0;i < elem[name].size() && i < size;i++)
		{
			data[i] = elem[name][i].asUInt();
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,int data[], uint32_t size)
	{
		for (uint32_t i = 0;i < elem[name].size() && i < size;i++)
		{
			data[i] = elem[name][i].asInt();
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,uint32_t data[], uint32_t size)
	{
		for (uint32_t i = 0;i < elem[name].size() && i < size;i++)
		{
			data[i] = elem[name][i].asUInt();
		}
	}
	inline void Json2Data(Json::Value& elem,const std::string& name,int64_t data[], uint32_t size)
	{
		if (elem.isMember(name))
		{
			for (uint32_t i = 0;i < elem[name].size() && i < size;i++)
			{
				data[i] = strto64_q(elem[name][i].asCString(),NULL,10);
			}
		}
	}
	inline void Json2Data(const Json::Value& elem, const std::string& name, std::vector<int>& _v)
	{
		if (elem.isMember(name))
		{
			for (int i = 0; i < elem[name].size(); ++i)
			{
				_v.push_back(elem[name][i].asInt());
			}
		}
	}
	// 	inline void Json2Data(const Json::Value& elem, const std::string& name, vector<stAwardItem>& _v)
	// 	{
	// 		if (elem.isMember(name))
	// 		{
	// 			int id,count;
	// 			for (int i = 0; i < elem[name].size(); ++i)
	// 			{
	// 				id = elem[name][i]["id"].asInt();
	// 				count = elem[name][i]["count"].asInt();
	// 				stAwardItem item(id, count);
	// 				_v.push_back(item);
	// 			}
	// 		}
	// 	}
	inline void Json2Data(const Json::Value& elem, const std::string& name, std::map<int, int>& _m)
	{
		if (elem.isMember(name))
		{
			int key, value;
			for (int i = 0; i < elem[name].size(); ++i)
			{
				key = elem[name][i]["key"].asInt();
				value = elem[name][i]["value"].asInt();
				_m.insert(std::make_pair(key,value));
			}
		}
	}

}


#define D2J_BASE(root,pram,member)	\
	JSONTRANS::Data2Json(root,#member,pram.member)

#define J2D_BASE(root,pram,member)	\
	JSONTRANS::Json2Data(root,#member,pram.member)

#define D2J_BASE_NAME(root,pram,member,name)	\
	JSONTRANS::Data2Json(root,name,pram.member)

#define J2D_BASE_NAME(root,pram,member,name)	\
	JSONTRANS::Json2Data(root,name,pram.member)

#define D2J_ARRAY(root,pram,member,size)	\
	JSONTRANS::Data2Json(root,#member,pram.member,size)

#define J2D_ARRAY(root,pram,member,size)	\
	JSONTRANS::Json2Data(root,#member,pram.member,size)

#define D2J_ARRAY_NAME(root,pram,member,size,name)	\
	JSONTRANS::Data2Json(root,name,pram.member,size)

#define J2D_ARRAY_NAME(root,pram,member,size,name)	\
	JSONTRANS::Json2Data(root,name,pram.member,size)

#define J2D_READ(root,name,member) \
	JSONTRANS::Json2Data(root,name,member)

#define J2D_SAVE(root,name,member) \
	JSONTRANS::Data2Json(root,name,member)

#define ARRAY_READ(root,name,member,size) \
	JSONTRANS::Json2Data(root,name,member,size)

#define ARRAY_SAVE(root,name,member,size) \
	JSONTRANS::Data2Json(root,name,member,size)

#define D2J(pram,member)	\
	D2J_BASE(elem_value,pram,member)

#define J2D(pram,member)	\
	J2D_BASE(elem_value,pram,member)

#define D2J_N(pram,member,name)	\
	D2J_BASE_NAME(elem_value,pram,member,name)

#define J2D_N(pram,member,name)	\
	J2D_BASE_NAME(elem_value,pram,member,name)

#define D2J_A(pram,member,size)	\
	D2J_ARRAY(elem_value,pram,member,size)

#define J2D_A(pram,member,size)	\
	J2D_ARRAY(elem_value,pram,member,size)

#define D2J_A_N(pram,member,size,name)	\
	D2J_ARRAY_NAME(elem_value,pram,member,size,name)

#define J2D_A_N(pram,member,size,name)	\
	J2D_ARRAY_NAME(elem_value,pram,member,size,name)