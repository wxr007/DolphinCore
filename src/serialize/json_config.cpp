#include <fstream>
#include <stdlib.h> 
#include "serialize/json_config.h"
#include "logging/logging/zLogger.h"

using std::string;

bool JsonConfig::LoadFile(const std::string& file_name)
{
	if (!file_name.empty())
	{
		std::ifstream ifs;
		ifs.open(file_name.c_str());

		if (ifs.is_open())
		{
			Json::Reader reader;
			if (reader.parse(ifs,root_,false))
			{
				return true;
			}else{
				g_logger.error("json file parse failed!");
				//json解析失败
			}
		}else{
			g_logger.error("json file read failed!");
			//json文件读取失败
		}
	}
	return false;
}

bool JsonConfig::GetExIPPort( const std::string& svrname,int32_t port,IpPortList& ip_ports )
{
	ip_ports.clear();

	if (!root_.isNull())
	{
		Json::Value svr_array = root_[svrname];//获得相应的服务器角色的列表
		if (!svr_array.isArray()) {return false;}

		for (int i = 0; i <svr_array.size(); ++i)
		{
			Json::Value port_array = svr_array[i]["port"]; //找到端口列表
			if (!port_array.isArray()) {return false;}

			for(int j = 0; j < port_array.size();j++)
			{
				if (!port_array[j]["internalPort"].isString()) {return false;}

				std::string innerport_str = port_array[j]["internalPort"].asString();
				int32_t innerport = atoi(innerport_str.c_str());
				if (innerport == port)//需要查询的内部端口相等
				{
					if (port_array[j]["externalPort"].isString())//判断外部端口
					{
						std::string externport_str = port_array[j]["externalPort"].asString();
						stIpPort ip_port;
						ip_port.port_ = atoi(externport_str.c_str());
						ip_port.ip_ = svr_array[i]["ip"].asString();
						ip_port.svr_id_ = svr_array[i]["cid"].asString();
						ip_ports.push_back(ip_port);
					}
				}
			}
		}
	}

	if (ip_ports.size() > 0)
	{
		return true;
	}
	return false;
}

bool JsonConfig::GetExtraData(const std::string& svrname,const string &svrid,stExtra & out)
{
    bool ret = false;
    if (!root_.isNull())
    {
        Json::Value svr_array = root_[svrname];//获得相应的服务器角色的列表
        if (!svr_array.isArray()) {return false;}

        for (int i = 0; i <svr_array.size(); ++i)
        {
            Json::Value jsonextra = svr_array[i]["extra"]; //找到端口列表
            out.zoneid_ = atoi(jsonextra["zoneid"].asCString());
            out.platform_name_ = jsonextra["platform"].asString();
            out.project_name_ = jsonextra["project"].asString();
            if (svrid == svr_array[i]["cid"].asString())
            {//取到了对应的数据,break.
                ret = true;
                break;
            }
        }
    }
    return ret;
}
