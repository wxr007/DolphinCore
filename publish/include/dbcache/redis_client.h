#pragma once
#include "nosql_define.h"
#include <hiredis.h>
#include <string>
#include "redisArg.h"

namespace QCOOL
{

	class RedisSyncClient		//Redis同步客户端
	{
	public:
		RedisSyncClient(const NoSqlDBConfig& redis_connect_info);
		~RedisSyncClient();
		/*--->[连接redis服务器]*/
		bool RedisConnect();//连接redis服务器
		/*--->[保存到redis hash中 set_table_key: 为主键 set_value中 nosql_field : 表示字段名 set_value中 nosql_value : 表示字段值]*/
		void RedisSetHash(const char* set_table_key, NoSqlValueVector& set_value);
		/*--->[从redis hash中取出 get_table_key: 为主键 get_value中 nosql_field : 表示字段名 get_value中 nosql_value : 表示字段值]*/
		void RedisGetHash(const char* get_table_key, NoSqlValueVector& get_value);
		void RedisGetHashAll(const char* get_table_key, NoSqlValueVector& get_value);

		/*--->[取出所有的key]*/
		void RedisGetAllKeys(const char* get_table_key, NoSqlKeyVector& get_keys);
		void RedisAddSet(const char* set_table_key, NoSqlKeyVector& set_keys);
		void RedisGetSet(const char* get_table_key, NoSqlKeyVector& get_keys);
		//发布
		//channel: 发布的频道
		//void RedisPublish(const char* channel, NoSqlKeyVector& pub_keys);
		//订阅
		//channel: 订阅的频道
		//void RedisSubscribe(const char* channel, NoSqlKeyVector& sub_keys);
	public:
		/*
		@定义:释放redis-context
		*/
		void RedisFreeContext();


		/*--->[服务端间数据同步发布频道接口]*/
		bool RedisPacketPublish(const char *szChannel,char *pBuf,int nLen);
		/*--->[服务端间数据同步订阅频道接口]*/
		void RedisPacketSubScribe(const char *szChannel, std::string & strOutPut, int &nLen);

		/*--->[服务端数据同步获取接口]*/
		bool RedisBigStringGetHash(const char* get_table_key, std::string & strOutPut, int &nLen);
		/*--->[服务端数据同步设置接口]*/
		bool RedisBigStringSetHash(const char* set_table_key, std::string & strInPut);
		/*--->[服务端删除Redis数据]*/
		bool RedisDeleteData(const char* del_table_key);
		/*--->[模糊查询Keys 列表]*/
		bool RedisKeysData(const char* keys_table_key, std::string &strOutPut);
		/*--->[查询Key值是否存在]*/
		bool RedisExistKey(const char* keys_table_key, int &nCount);
		/*
		@定义:设置子集值
		@参数:set_table_key 关键值 strInPut 具体数值
		*/
		void RedisZAddData(const char* set_table_key, std::string & strInPut);
		//
		void* RedisCmd(RedisCmdArg &cmdarg);
		void FreeRedisObj(void *obj);

		/*
		@定义:设置子集值
		@参数:set_table_key 关键值 strInPut 具体数值
		*/
		void RedisBigStringUpdataSet(const char* set_table_key, std::string & strInPut);
		
	private:
		NoSqlDBConfig		m_connect_info;
		redisContext*		m_redis_context;
		redisReply*			m_redis_reply;
		bool				m_is_subscribe;
		RedisCmdArg         m_redis_cmd_arg;
	};
}
