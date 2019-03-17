#pragma once
#include "redis_client.h"
#include <string>

namespace QCOOL
{
	class	NoSqlDBCache
	{
	public:
		NoSqlDBCache();
		~NoSqlDBCache();
		/*--->[按字段映射关系读取数据]*/
		void SetDataCache(const char* set_table_key, NoSqlValueVector& set_value);
		void GetDataCache(const char* get_table_key, NoSqlValueVector& get_value);
		void GetDataCacheAll(const char* get_table_key, NoSqlValueVector& get_value);
		void GetAllKeys(const char* get_table_key, NoSqlKeyVector& get_value);
		void AddSetCache(const char* set_table_key,NoSqlKeyVector& set_value);
		void GetSetCache(const char* get_table_key,NoSqlKeyVector& get_value);
		//void Publish(const char* channel,NoSqlKeyVector& pub_keys);
		//void Subscribe(const char* channel,NoSqlKeyVector& sub_keys);

		/*--->[读取基础config配置]*/
		bool LoadConfig(const NoSqlDBConfig& config);
		/*--->[链接Redis]*/
		bool Open();

		/*--->[重新连接redis对象]*/
		bool ReConnectObj(RedisSyncClient *pClient);

		/*--->[服务端数据发布频道接口]*/
		void PacketSubScribe(const char *szChannel, std::string & strOutPut, int & nLen);
		/*--->[服务端数据发布频道接口]*/
		void PacketPublish(const char *szChannel, char *pBuf, int nLen);
		/*--->[服务端redis更新数据]*/
		void SetRedisData(const char* set_table_key, std::string & strInPut);
		/*--->[服务端Redis获取数据]*/
		void GetRedisData(const char* get_table_key, std::string & strOutPut, int &nLen);
		/*--->[服务端删除Redis获取数据]*/
		void DelRedisData(const char* del_table_key);
		/*--->[获得键值模糊查询键值列表]*/
		void KeysRedisData(const char* keys_table_key, std::string & strOutPut);
		/*--->[查询Field是否存在]*/
		void ExistRedisKey(const char* keys_table_key, int &nCount);
		/*
		@定义:设置子集值
		@参数:set_table_key 关键值 strInPut 具体数值
		*/
		void ZAddRedisData(const char* set_table_key, std::string & strInPut);
	private:
		RedisSyncClient*  nosql_db_cache;
		RedisSyncClient*  subscribe_client;
	};
}

