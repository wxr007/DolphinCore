#ifndef DEF_JSON_CONFIG_H
#define DEF_JSON_CONFIG_H

#include <stdint.h>
#include <string>
#include <vector>
#include "jsoncpp/json/json.h"

#define  CONFIG_FILE			"./JsonConfig/SvrConfig.json"		//配置文件名字

struct stIpPort
{
	std::string svr_id_;		//外部服务器id
	std::string ip_;			//外部服务器ip
	int32_t port_;				//外部服务器端口
	stIpPort()
	{
		svr_id_.clear();
		ip_.clear();
		port_ = 0;
	}
};

struct stExtra
{
    int32_t zoneid_;
    std::string  platform_name_;
    std::string  project_name_;
};

typedef std::vector<stIpPort>  IpPortList;

class JsonConfig
{
public:
	JsonConfig(){}
	~JsonConfig(){}
	/*-->[ 读取配置文件 ]*/
	bool LoadFile(const std::string& file_name);
	/*-->[ 获得外部端口 ]*/
	bool GetExIPPort(const std::string& svrname,int32_t port,IpPortList& ip_ports);
    /*-->[ 获得extra部分数据 ]*/
    bool GetExtraData(const std::string& svrname,const std::string &svrid,stExtra & out);
private:
	Json::Value  root_;
};



#endif //DEF_JSON_CONFIG_H