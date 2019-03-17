#pragma once
#include <stdint.h>
#include <vector>
#include <string>

/*--->[字符串数组]*/
typedef std::vector<std::string>		StringArray;
/*--->[src_str：被切分字符串 split_str：分隔符 out_array：切分返回的字符串数组]*/
void StringSplit(std::string& src_str, std::string delimiter, StringArray& out_array);
/*--->[除去字符串收尾空白 src_str：需要除去空白的字符串]*/
void StringTrim(std::string& src_str);

namespace QCOOL
{
	struct NoSqlDBConfig
	{
		std::string	nosql_ip;
		int16_t nosql_port;
		NoSqlDBConfig()
			:nosql_ip("")
			,nosql_port(0)
		{
		}
		NoSqlDBConfig(const std::string& ip,int16_t port)
			:nosql_ip(ip)
			,nosql_port(port)
		{

		}
		NoSqlDBConfig(const NoSqlDBConfig& new_data)
			:nosql_ip(new_data.nosql_ip)
			,nosql_port(new_data.nosql_port)
		{
		}
	};

	struct	NoSqlDBKeyValue
	{
		std::string	nosql_field;
		std::string	nosql_value;
	};

	typedef std::vector<NoSqlDBKeyValue>	NoSqlValueVector;
	typedef StringArray			NoSqlKeyVector;
}