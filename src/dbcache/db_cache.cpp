#include "dbcache/db_cache.h"
#include "logging/logging/zLogger.h"

namespace QCOOL
{

	NoSqlDBCache::NoSqlDBCache()
		:nosql_db_cache(NULL)
		, subscribe_client(NULL)
	{

	}

	NoSqlDBCache::~NoSqlDBCache()
	{
	}

	bool NoSqlDBCache::LoadConfig(const NoSqlDBConfig& config)
	{
		if (nosql_db_cache == NULL && subscribe_client == NULL)
		{
			nosql_db_cache = new RedisSyncClient(config);
			subscribe_client = new RedisSyncClient(config);
			if (nosql_db_cache && subscribe_client)
			{
				return true;
			}
			if (nosql_db_cache)
			{
				delete nosql_db_cache;
				nosql_db_cache = NULL;
			}
			if (subscribe_client)
			{
				delete subscribe_client;
				subscribe_client = NULL;
			}
		}
		return false;
	}

	/*--->[重新连接redis对象]*/
	bool NoSqlDBCache::ReConnectObj(RedisSyncClient *pClient){
		pClient->RedisFreeContext();
		if (pClient->RedisConnect() == true){
			g_logger.info("[redis][info]:redis reconnect success!");
			return true;
		}
		g_logger.error("[redis][info]:redis reconnect fail!");
		return false;
	}

	bool NoSqlDBCache::Open()
	{
		if (nosql_db_cache && subscribe_client)
		{
			if (nosql_db_cache->RedisConnect() && subscribe_client->RedisConnect())
			{
				return true;
			}
		}
		return false;
	}

	void NoSqlDBCache::SetDataCache(const char* set_table_key, NoSqlValueVector& set_value)
	{
		if (nosql_db_cache)
		{
			nosql_db_cache->RedisSetHash(set_table_key, set_value);
		}
	}

	void NoSqlDBCache::GetDataCache(const char* get_table_key, NoSqlValueVector& get_value)
	{
		if (nosql_db_cache)
		{
			nosql_db_cache->RedisGetHash(get_table_key, get_value);
		}
	}

	void NoSqlDBCache::GetDataCacheAll(const char* get_table_key, NoSqlValueVector& get_value)
	{
		if (nosql_db_cache)
		{
			nosql_db_cache->RedisGetHashAll(get_table_key, get_value);
		}
	}

	void NoSqlDBCache::GetAllKeys(const char* get_table_key, NoSqlKeyVector& get_value)
	{
		if (nosql_db_cache)
		{
			nosql_db_cache->RedisGetAllKeys(get_table_key, get_value);
		}
	}

	void NoSqlDBCache::AddSetCache(const char* set_table_key, NoSqlKeyVector& set_value)
	{
		if (nosql_db_cache)
		{
			nosql_db_cache->RedisAddSet(set_table_key, set_value);
		}
	}

	void NoSqlDBCache::GetSetCache(const char* get_table_key, NoSqlKeyVector& get_value)
	{
		if (nosql_db_cache)
		{
			nosql_db_cache->RedisGetSet(get_table_key, get_value);
		}
	}

//	void NoSqlDBCache::Publish(const char* channel, NoSqlKeyVector& pub_keys)
//	{
//		if (nosql_db_cache)
//		{
//			nosql_db_cache->RedisPublish(channel, pub_keys);
//		}
//	}
//
//	void NoSqlDBCache::Subscribe(const char* channel, NoSqlKeyVector& sub_keys)
//	{
//		if (subscribe_client)
//		{
//			subscribe_client->RedisSubscribe(channel, sub_keys);
//		}
//	}

	void NoSqlDBCache::PacketSubScribe(const char *szChannel, std::string & strOutPut, int & nLen)
	{
		if (subscribe_client)
		{
			subscribe_client->RedisPacketSubScribe(szChannel, strOutPut, nLen);
		}
	}

	void NoSqlDBCache::PacketPublish(const char *szChannel, char *pBuf, int nLen)
	{
		if (nosql_db_cache)
		{
			if (nosql_db_cache->RedisPacketPublish(szChannel, pBuf, nLen) == false){
				ReConnectObj(nosql_db_cache);
				if (nosql_db_cache->RedisPacketPublish(szChannel, pBuf, nLen) == false){
					g_logger.error("[redis][error]:PacketPublish after connect senddata fail!");
				}
			}
		}
	}

	/*--->[服务端redis更新数据]*/
	void NoSqlDBCache::SetRedisData(const char* set_table_key, std::string & strInPut)
	{
		if (nosql_db_cache)
		{
			if (nosql_db_cache->RedisBigStringSetHash(set_table_key, strInPut) == false){
				ReConnectObj(nosql_db_cache);
				if (nosql_db_cache->RedisBigStringSetHash(set_table_key, strInPut) == false){
					g_logger.error("[redis][error]:RedisBigStringSetHash after connect senddata fail!");
				}
			}
		}
	}
	/*--->[服务端Redis获取数据]*/
	void NoSqlDBCache::GetRedisData(const char* get_table_key, std::string & strOutPut, int &nLen)
	{
		if (nosql_db_cache)
		{
			if (nosql_db_cache->RedisBigStringGetHash(get_table_key, strOutPut, nLen) == false){
				ReConnectObj(nosql_db_cache);
				if (nosql_db_cache->RedisBigStringGetHash(get_table_key, strOutPut, nLen) == false){
					g_logger.error("[redis][error]:RedisBigStringGetHash after connect senddata fail!");
				}
			}
		}
	}

	/*--->[服务端删除Redis获取数据]*/
	void NoSqlDBCache::DelRedisData(const char* del_table_key)
	{
		if (nosql_db_cache)
		{
			if (nosql_db_cache->RedisDeleteData(del_table_key) == false){
				ReConnectObj(nosql_db_cache);
				if (nosql_db_cache->RedisDeleteData(del_table_key) == false){
					g_logger.error("[redis][error]:RedisDeleteData after connect senddata fail!");
				}
			}
		}
	}
	/*--->[获得键值模糊查询键值列表]*/
	void NoSqlDBCache::KeysRedisData(const char* keys_table_key, std::string & strOutPut)
	{
		if (nosql_db_cache)
		{
			if (nosql_db_cache->RedisKeysData(keys_table_key, strOutPut) == false){
				ReConnectObj(nosql_db_cache);
				if (nosql_db_cache->RedisKeysData(keys_table_key, strOutPut) == false){
					g_logger.error("[redis][error]:RedisKeysData after connect senddata fail!");
				}
			}
		}
	}
	
	/*--->[查询Field是否存在]*/
	void NoSqlDBCache::ExistRedisKey(const char* keys_table_key, int &nCount)
	{
		if (nosql_db_cache)
		{
			if (nosql_db_cache->RedisExistKey(keys_table_key, nCount) == false){
				ReConnectObj(nosql_db_cache);
				if (nosql_db_cache->RedisExistKey(keys_table_key, nCount) == false){
					g_logger.error("[redis][error]:RedisKeysData after connect senddata fail!");
				}
			}
		}
	}

	/*
	@定义:设置子集值
	@参数:set_table_key 关键值 strInPut 具体数值
	*/
	void NoSqlDBCache::ZAddRedisData(const char* set_table_key, std::string & strInPut){
		if (nosql_db_cache)
		{
			nosql_db_cache->RedisZAddData(set_table_key, strInPut);
		}
	}
}