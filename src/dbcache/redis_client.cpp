#include "dbcache/redis_client.h"
#include "logging/logging/zLogger.h"
//#include "log_adaptor.h"

namespace QCOOL
{
    //redis cmd;
    const string REDIS_CMD_HMSET("HMSET");
    const string REDIS_CMD_HMGET("HMGET");
    const string REDIS_CMD_HGETALL("HGETALL");
    const string REDIS_CMD_KEYS("KEYS");
    const string REDIS_CMD_EXISTS("EXISTS");
    const string REDIS_CMD_SADD("SADD");
    const string REDIS_CMD_SMEMBERS("SMEMBERS");
    const string REDIS_CMD_PUBLISH("PUBLISH");
    const string REDIS_CMD_SUBSCRIBE("SUBSCRIBE");
	const string REDIS_CMD_GET("GET");
	const string REDIS_CMD_SET("SET");
	const string REDIS_CMD_DEL("DEL");	
	const string REDIS_CMD_ZADD("ZADD");


	const char* GetRedisStrType(int type_num)
	{
		switch(type_num)
		{
		case REDIS_REPLY_STRING:
			return "REDIS_REPLY_STRING";
			break;
		case REDIS_REPLY_ARRAY:
			return "REDIS_REPLY_ARRAY";
			break;
		case REDIS_REPLY_INTEGER:
			return "REDIS_REPLY_INTEGER";
			break;
		case REDIS_REPLY_NIL:
			return "REDIS_REPLY_NIL";
			break;
		case REDIS_REPLY_STATUS:
			return "REDIS_REPLY_STATUS";
			break;
		case REDIS_REPLY_ERROR:
			return "REDIS_REPLY_ERROR";
			break;
		default:
			return "unknow redis type";
		}
	}

	//===============================================================
	RedisSyncClient::RedisSyncClient(const NoSqlDBConfig& connect_info)
		:m_connect_info(connect_info)
		, m_redis_context(NULL)
		, m_is_subscribe(false)
	{

	}

	RedisSyncClient::~RedisSyncClient()
	{
		if (m_redis_reply)
		{
			freeReplyObject(m_redis_reply);
		}
		if (m_redis_context)
		{
			redisFree(m_redis_context);
		}
	}

	bool RedisSyncClient::RedisConnect()
	{
		if (m_redis_context == NULL)
		{
			m_redis_context = redisConnect(m_connect_info.nosql_ip.c_str(), m_connect_info.nosql_port);
			if (m_redis_context == NULL || m_redis_context->err)
			{
				if (m_redis_context)
				{
					g_logger.debug("redis connect: %s", m_redis_context->errstr);
					redisFree(m_redis_context);
					m_redis_context = NULL;
				}
				else
				{
					g_logger.debug("redis connect: can't allocate redis context.");
				}
				return false;
			}
			g_logger.info("redis connect success!");
			return true;
		}
		return false;
	}
	void RedisSyncClient::RedisSetHash(const char* set_table_key, NoSqlValueVector& set_value)
	{
	    if (m_redis_context)
	    {
	        int32_t argnum = 1 + 1+2 * set_value.size();
	        m_redis_cmd_arg.InitRedisCmdArg(argnum);
	        //1-cmd;
	        m_redis_cmd_arg.AppendArg(REDIS_CMD_HMSET);
	        //2-key;values;
	        m_redis_cmd_arg.AppendArg(set_table_key,strlen(set_table_key));
	        for (int i = 0; i < set_value.size(); i++)
	        {
	            m_redis_cmd_arg.AppendArg(set_value[i].nosql_field);
	            m_redis_cmd_arg.AppendArg(set_value[i].nosql_value);
	        }
	        g_logger.debug("RedisSetHash HMSET:%s (%d)",set_table_key,set_value.size());
	        m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));
	        FreeRedisObj(m_redis_reply);
        }
	}

	void RedisSyncClient::RedisGetHash(const char* get_table_key, NoSqlValueVector& get_value)
	{
	    if (m_redis_context)
	    {
	        int32_t argnum = 1 + 1+get_value.size();
	        m_redis_cmd_arg.InitRedisCmdArg(argnum);
	        //1-cmd;
	        m_redis_cmd_arg.AppendArg(REDIS_CMD_HMGET);
	        //2-args;
	        m_redis_cmd_arg.AppendArg(get_table_key,strlen(get_table_key));
	        for (int i = 0; i < get_value.size(); i++)
	        {
	            m_redis_cmd_arg.AppendArg(get_value[i].nosql_field);
	        }
	        g_logger.debug("RedisGetHash HMGET:%s (%d)",get_table_key,get_value.size());
	        m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));
	        
			if (m_redis_reply)
			{
                switch (m_redis_reply->type)
                {
                case REDIS_REPLY_ERROR:
                {
                    g_logger.debug("redis HMGET error: %s", m_redis_reply->str);
                }
                	break;
                case REDIS_REPLY_ARRAY:		//hash返回出来是Array
                {
                	for (int i = 0; i < m_redis_reply->elements || i < get_value.size(); i++)
                	{
                		g_logger.debug("value: %s", m_redis_reply->element[i]->str);

                		get_value[i].nosql_value.append(m_redis_reply->element[i]->str, m_redis_reply->element[i]->len);
                	}
                }
                	break;
                }
				FreeRedisObj(m_redis_reply);
				m_redis_reply = NULL;
		    }
		}
	}

	void RedisSyncClient::RedisGetHashAll(const char* get_table_key, NoSqlValueVector& get_value)
	{
		if (m_redis_context)
		{
		    int32_t argnum = 1 + 1;
		    m_redis_cmd_arg.InitRedisCmdArg(argnum);
		    //1-cmd;
	        m_redis_cmd_arg.AppendArg(REDIS_CMD_HGETALL);
	        //2-args;
	        m_redis_cmd_arg.AppendArg(get_table_key,strlen(get_table_key));
	        g_logger.debug("RedisGetHashAll HGETALL:%s ~",get_table_key);
	        
			m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));
			if (m_redis_reply)
			{
				get_value.clear();
				switch (m_redis_reply->type)
				{
				case REDIS_REPLY_ERROR:
				{
					g_logger.debug("redis HGETALL error: %s", m_redis_reply->str);
				}
					break;
				case REDIS_REPLY_ARRAY:		//hash返回出来是Array
				{
                    get_value.reserve(m_redis_reply->elements / 2);

                    NoSqlDBKeyValue	key_value;

                    for (int i = 0; i < m_redis_reply->elements; i++)
                    {
                    	if (i % 2 == 0)		//key
                    	{
                    		key_value.nosql_field.clear();
                    		key_value.nosql_field.append(m_redis_reply->element[i]->str, m_redis_reply->element[i]->len);
                    	}
                    	else				//value
                    	{
                    		key_value.nosql_value.clear();
                    		key_value.nosql_value.append(m_redis_reply->element[i]->str, m_redis_reply->element[i]->len);
                    		get_value.push_back(key_value);
                    	}
                    }
				}
					break;
				}
				FreeRedisObj(m_redis_reply);
				m_redis_reply = NULL;
			}
		}
	}

	void RedisSyncClient::RedisGetAllKeys(const char* get_table_key, NoSqlKeyVector& get_keys)
	{
		if (m_redis_context)
		{
			if (get_table_key)
			{
    		    int32_t argnum = 1 + 1;
    		    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    		    //1-cmd;
    	        m_redis_cmd_arg.AppendArg(REDIS_CMD_KEYS);
    	        //2-args;
    	        m_redis_cmd_arg.AppendArg(get_table_key,strlen(get_table_key));
    	        g_logger.debug("RedisGetAllKeys KEYS:%s!",get_table_key);
    	        
    	        m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));
			}
			else
			{
				m_redis_reply = static_cast<redisReply*>(redisCommand(m_redis_context, "KEYS  *"));
			}

			if (m_redis_reply)
			{
				switch (m_redis_reply->type)
				{
				case REDIS_REPLY_ERROR:
				{
                    g_logger.debug("redis KEYS error: %s", m_redis_reply->str);
				}
					break;
				case REDIS_REPLY_ARRAY:		//keys返回出来是Array
				{
                    get_keys.reserve(m_redis_reply->elements);

                    std::string	temp_key;

                    for (int i = 0; i < m_redis_reply->elements; i++)
                    {
                        temp_key.clear();
                        temp_key.append(m_redis_reply->element[i]->str, m_redis_reply->element[i]->len);
                        get_keys.push_back(temp_key);
                    }
				}
					break;
				}

				FreeRedisObj(m_redis_reply);
				m_redis_reply = NULL;
			}
		}
	}

	void RedisSyncClient::RedisAddSet(const char* set_table_key, NoSqlKeyVector& set_keys)
	{
	    if (m_redis_context && set_table_key)
	    {
    	    int32_t argnum = 1 + 1 + set_keys.size();
    	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    	    //1-cmd;
            m_redis_cmd_arg.AppendArg(REDIS_CMD_SADD);
            //2-args;
            m_redis_cmd_arg.AppendArg(set_table_key,strlen(set_table_key));	
            m_redis_cmd_arg.AppendArg(set_keys);
            g_logger.debug("RedisAddSet SADD:%s (%d)!",set_table_key,set_keys.size());

            m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));            

            FreeRedisObj(m_redis_reply);
        }
	}

	void RedisSyncClient::RedisGetSet(const char* get_table_key, NoSqlKeyVector& get_keys)
	{
	    if (m_redis_context && get_table_key)
	    {
    	    int32_t argnum = 1 + 1;
    	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    	    //1-cmd;
            m_redis_cmd_arg.AppendArg(REDIS_CMD_SMEMBERS);
            //2-args;
            m_redis_cmd_arg.AppendArg(get_table_key,strlen(get_table_key));	
            g_logger.debug("RedisGetSet SMEMBERS:%s !",get_table_key);

            m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));
            if (m_redis_reply)
            {
                if (REDIS_REPLY_ARRAY == m_redis_reply->type)
                {
    				get_keys.reserve(m_redis_reply->elements);
    				std::string	temp_key;

    				for (int i = 0; i < m_redis_reply->elements; i++)
    				{
    					temp_key.clear();
    					temp_key.append(m_redis_reply->element[i]->str, m_redis_reply->element[i]->len);
    					get_keys.push_back(temp_key);
    				}                
                }
                FreeRedisObj(m_redis_reply);
            }
        }
	}

//	void RedisSyncClient::RedisPublish(const char* channel, NoSqlKeyVector& pub_keys)
//	{
//		if (m_redis_context && channel)
//		{
//			std::string	 set_command = "PUBLISH ";
//			set_command += channel;
//			set_command += " ";
//
//			for (int i = 0; i < pub_keys.size(); i++)
//			{
//				set_command += pub_keys[i];
//				set_command += ";";
//			}
//			g_logger.info("redis PUBLISH: %s", set_command.c_str());
//
//			m_redis_reply = static_cast<redisReply*>(redisCommand(m_redis_context, set_command.c_str()));
//
//			if (m_redis_reply)
//			{
//				switch (m_redis_reply->type)
//				{
//				case REDIS_REPLY_ERROR:
//				{
//										  g_logger.debug("redis PUBLISH : %s", m_redis_reply->str);
//				}
//					break;
//				}
//				freeReplyObject(m_redis_reply);
//				m_redis_reply = NULL;
//			}
//		}
//	}

//	void RedisSyncClient::RedisSubscribe(const char* channel, NoSqlKeyVector& sub_keys)
//	{
//		if (m_redis_context && channel)
//		{
//			if (!m_is_subscribe)
//			{
//				std::string	 get_command = "SUBSCRIBE ";
//				get_command += channel;
//
//				g_logger.info("redis SUBSCRIBE: %s", get_command.c_str());
//
//				m_redis_reply = static_cast<redisReply*>(redisCommand(m_redis_context, get_command.c_str()));
//
//				if (m_redis_reply)
//				{
//					switch (m_redis_reply->type)
//					{
//					case REDIS_REPLY_ERROR:
//					{
//											  g_logger.debug("redis SUBSCRIBE : %s", m_redis_reply->str);
//					}
//						break;
//					}
//					freeReplyObject(m_redis_reply);
//					m_redis_reply = NULL;
//					m_is_subscribe = true;
//				}
//			}
//
//			void *_reply = NULL;
//
//			if (redisGetReply(m_redis_context, &_reply) == REDIS_OK)
//			{
//				m_redis_reply = static_cast<redisReply*>(_reply);
//				if (m_redis_reply)
//				{
//					switch (m_redis_reply->type)
//					{
//					case REDIS_REPLY_ERROR:
//					{
//												g_logger.debug("redis SUBSCRIBE wait : %s", m_redis_reply->str);
//					}
//						break;
//					case REDIS_REPLY_ARRAY:
//					{
//						// 							sub_keys.reserve(m_redis_reply->elements);
//						if (m_redis_reply->elements >= 3)
//						{
//							std::string	temp_key(m_redis_reply->element[2]->str, m_redis_reply->element[2]->len);
//							StringSplit(temp_key, ";", sub_keys);
//						}
//					}
//						break;
//					}
//				}
//			}
//
//			if (m_redis_reply)
//			{
//				freeReplyObject(m_redis_reply);
//				m_redis_reply = NULL;
//			}
//		}
//	}
	/*
	@定义:释放redis-context
	*/
	void RedisSyncClient::RedisFreeContext()
	{
		redisFree(m_redis_context);
		m_redis_context = NULL;
		g_logger.info("[redis][info]:free m_redis_context success!");
	}

/*--->[服务端数据同步发布频道接口]*/
	bool RedisSyncClient::RedisPacketPublish(const char *szChannel , char *pBuf, int nLen)
	{
		bool boRedisVilad = false;
		if (m_redis_context && szChannel)
		{
    	    int32_t argnum = 1+ 1 + 1;
    	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    	    //1-cmd;
            m_redis_cmd_arg.AppendArg(REDIS_CMD_PUBLISH);
            //2-args;channel and value;
            m_redis_cmd_arg.AppendArg(szChannel,strlen(szChannel));
            m_redis_cmd_arg.AppendArg(pBuf,nLen);

            g_logger.debug("RedisPacketPublish PUBLISH:%s %s!",szChannel,pBuf);

            m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));
			if (m_redis_reply)
			{
				if (m_redis_reply->type != REDIS_REPLY_ERROR)
				{
					boRedisVilad = true;
				}
			}
            FreeRedisObj(m_redis_reply);
			
		}
		return boRedisVilad;
	}

	/*--->[服务端数据同步订阅频道接口]*/
	void RedisSyncClient::RedisPacketSubScribe(const char *szChannel, std::string & strOutPut, int &nLen)
	{
		if (m_redis_context && szChannel)
		{
			if (!m_is_subscribe)
			{
        	    int32_t argnum = 1+ 1;
        	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
        	    //1-cmd;
                m_redis_cmd_arg.AppendArg(REDIS_CMD_SUBSCRIBE);
                //2-args;channel;
                m_redis_cmd_arg.AppendArg(szChannel,strlen(szChannel));
                g_logger.debug("RedisPacketSubScribe SUBSCRIBE:%s!",szChannel);

                m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));

                FreeRedisObj(m_redis_reply);
				m_is_subscribe = true;
			}

			void *_reply = NULL;

			if (redisGetReply(m_redis_context, &_reply) == REDIS_OK)
			{
				m_redis_reply = static_cast<redisReply*>(_reply);
				if (m_redis_reply)
				{
					switch (m_redis_reply->type)
					{
						case REDIS_REPLY_ERROR:
						{
							g_logger.debug("redis SUBSCRIBE wait : %s", m_redis_reply->str);
						}
						break;
						case REDIS_REPLY_ARRAY:
						{
							if (m_redis_reply->elements >= 3)
							{
								std::string strTemp(m_redis_reply->element[2]->str, m_redis_reply->element[2]->len);
								strOutPut.clear();
								strOutPut = strTemp;
								if (strTemp.length() <= 0)
								{
									g_logger.debug("获得redis subcribe 数据 <=0");
								}
								//g_logger.debug("strOutPut %s strTemp %s", strOutPut.c_str(), strTemp.c_str());
								nLen = m_redis_reply->element[2]->len;
								//std::string	temp_key(m_redis_reply->element[2]->str, m_redis_reply->element[2]->len);
								//StringSplit(temp_key, ";", sub_keys);
							}
						}
						break;
					}
				}
			}

			if (m_redis_reply)
			{
				freeReplyObject(m_redis_reply);
				m_redis_reply = NULL;
			}
		}
	}

	/*--->[服务端数据同步订阅频道接口]*/
	bool RedisSyncClient::RedisBigStringGetHash(const char* get_table_key, std::string & strOutPut, int &nLen)
	{
		bool boRedisVilad = false;
		if (m_redis_context)
		{
			boRedisVilad = true;
			//init;
        	int32_t argnum = 1 + 1;
    	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    	    //1-cmd;
            m_redis_cmd_arg.AppendArg(REDIS_CMD_GET);
            //2-args;key;
            m_redis_cmd_arg.AppendArg(get_table_key,strlen(get_table_key));
            g_logger.debug("RedisBigStringGetHash GET:%s!",get_table_key);

            m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));

			if (m_redis_reply == NULL)
			{
				boRedisVilad = false;
			}
			else
			{
				switch (m_redis_reply->type)
				{
					case REDIS_REPLY_ERROR:
					{
						boRedisVilad = false;
						g_logger.debug("redis GET error: %s", m_redis_reply->str);
					}
					break;
					case REDIS_REPLY_STRING:
					{
							std::string	temp_key1(m_redis_reply->str, m_redis_reply->len);
							strOutPut = temp_key1;
							nLen = nLen = m_redis_reply->len;
					}
					break;
					default :
					{
					    g_logger.debug("redis GET unknow response type: [%d][%s]", m_redis_reply->type,GetRedisStrType(m_redis_reply->type));
					    break;
					}
				}
				freeReplyObject(m_redis_reply);
				m_redis_reply = NULL;
			}
		}
		return boRedisVilad;
	}
	/*--->[服务端数据同步订阅频道接口]*/
	bool RedisSyncClient::RedisBigStringSetHash(const char* set_table_key, std::string & strInPut)
	{
		bool boRedisVilad = false;
		if (m_redis_context)
		{
			boRedisVilad = true;
			//init;
        	int32_t argnum = 1 + 1 + 1;
    	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    	    //1-cmd;
            m_redis_cmd_arg.AppendArg(REDIS_CMD_SET);
            //2-args;key and value;
            m_redis_cmd_arg.AppendArg(set_table_key,strlen(set_table_key));
            m_redis_cmd_arg.AppendArg(strInPut);
            g_logger.debug("RedisBigStringSetHash SET:%s %s!",set_table_key,strInPut.c_str());

            m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));

			if (m_redis_reply == NULL)
			{
				boRedisVilad = false;
			}

			if (boRedisVilad == true){
				std::string set_UpdataName = "RedisUpdataTableSet";
				std::string set_UpdataKey = set_table_key;
				RedisBigStringUpdataSet(set_UpdataName.c_str(), set_UpdataKey);
			}
		}
		return boRedisVilad;
	}

	/*--->[服务端删除Redis数据]*/
	bool RedisSyncClient::RedisDeleteData(const char* del_table_key)
	{
		bool boRedisVilad = false;
		if (m_redis_context)
		{
			boRedisVilad = true;
        	int32_t argnum = 1 + 1;
    	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    	    //1-cmd;
            m_redis_cmd_arg.AppendArg(REDIS_CMD_DEL);
            //2-args;key;
            m_redis_cmd_arg.AppendArg(del_table_key,strlen(del_table_key));
            g_logger.debug("RedisDeleteData DEL:%s !",del_table_key);

            m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));
			if (m_redis_reply == NULL)
			{
				boRedisVilad = false;
			}
			FreeRedisObj(m_redis_reply);
		}
		return boRedisVilad;
	}

	/*--->[模糊查询Keys 列表]*/
	bool  RedisSyncClient::RedisKeysData(const char* keys_table_key, std::string &strOutPut)
	{
		bool boRedisVilad = false;
		if (m_redis_context)
		{
			boRedisVilad = true;
        	int32_t argnum = 1 + 1;
    	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    	    //1-cmd;
            m_redis_cmd_arg.AppendArg(REDIS_CMD_KEYS);
            //2-args;key;
            string tempkey = keys_table_key;
            tempkey += "_*";
            m_redis_cmd_arg.AppendArg(tempkey);
            g_logger.debug("RedisKeysData KEYS:%s !",tempkey.c_str());

            m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));

            g_logger.info("redis KEYS: %s", tempkey.c_str());
			
			if (m_redis_reply == NULL)
			{
				boRedisVilad = false;
			}
			if (m_redis_reply)
			{
				switch (m_redis_reply->type)
				{
				case REDIS_REPLY_ERROR:
					{
						g_logger.debug("[redis][error]redis KEYS error: %s", m_redis_reply->str);
						boRedisVilad = false;
					}
					break;
				case REDIS_REPLY_ARRAY:		//hash返回出来是Array
					{
						for (int i = 0; i < m_redis_reply->elements; i++)
						{
							strOutPut += m_redis_reply->element[i]->str;
							if (i + 1 < m_redis_reply->elements)
							{
								strOutPut += "|";
							}
						}
					}
					break;
				}
				freeReplyObject(m_redis_reply);
				m_redis_reply = NULL;
			}
		}
		return boRedisVilad;
	}

	/*--->[查询Key值是否存在]*/
	bool RedisSyncClient::RedisExistKey(const char* keys_table_key, int &nCount)
	{
		bool boRedisVilad = false;
		if (m_redis_context)
		{
			boRedisVilad = true;
        	int32_t argnum = 1 + 1;
    	    m_redis_cmd_arg.InitRedisCmdArg(argnum);
    	    //1-cmd;
            m_redis_cmd_arg.AppendArg(REDIS_CMD_EXISTS);
            //2-args;key;
            m_redis_cmd_arg.AppendArg(keys_table_key,strlen(keys_table_key));
            g_logger.debug("RedisExistKey EXISTS:%s !",keys_table_key);

            m_redis_reply = static_cast<redisReply*>(RedisCmd(m_redis_cmd_arg));
			
			g_logger.info("redis EXISTS: %s", keys_table_key);

			if (m_redis_reply == NULL)
			{
				boRedisVilad = false;
			}
			if (m_redis_reply)
			{
				switch (m_redis_reply->type)
				{
				case REDIS_REPLY_ERROR:
				{
										  g_logger.debug("[redis][error]redis KEYS error: %s", m_redis_reply->str);
										  boRedisVilad = false;
				}
					break;
				case REDIS_REPLY_INTEGER:		//hash返回出来是Array
				{
													nCount = m_redis_reply->integer;
				}
					break;
				}
				freeReplyObject(m_redis_reply);
				m_redis_reply = NULL;
			}
		}
		return boRedisVilad;
	}

	/*--->[服务端数据同步订阅频道接口]*/
	void RedisSyncClient::RedisZAddData(const char* set_table_key, std::string & strInPut)
	{
		if (m_redis_context)
		{
			std::string	 set_command = "ZADD ";
			set_command += set_table_key;
			set_command += ' ';
			set_command += strInPut.c_str();

			g_logger.info("redis ZADD: %s", set_command.c_str());

			m_redis_reply = static_cast<redisReply*>(redisCommand(m_redis_context, set_command.c_str()));
			if (m_redis_reply)
			{
				switch (m_redis_reply->type)
				{
				case REDIS_REPLY_ERROR:
				{
										  g_logger.debug("redis ZADD: %s", m_redis_reply->str);
				}
					break;
				}
				freeReplyObject(m_redis_reply);
				m_redis_reply = NULL;
			}
		}
	}

	void RedisSyncClient::RedisBigStringUpdataSet(const char* set_table_key, std::string & strInPut)
	{
		if (m_redis_context)
		{
			std::string	 set_command = "ZADD ";
			set_command += set_table_key;

			int64_t m_tempTime = GetTickCount64_Q();
			char szorder[128] = { 0 };
			sprintf(szorder, " %ld ", m_tempTime);

			set_command += szorder;
			set_command += strInPut.c_str();
			
			m_redis_reply = static_cast<redisReply*>(redisCommand(m_redis_context, set_command.c_str()));

			if (m_redis_reply)
			{
				switch (m_redis_reply->type)
				{
				case REDIS_REPLY_ERROR:
				{
					g_logger.debug("redis ZADD: %s", m_redis_reply->str);
				}
					break;
				}
				freeReplyObject(m_redis_reply);
				m_redis_reply = NULL;
			}
		}
	}

    void* RedisSyncClient::RedisCmd(RedisCmdArg &cmdarg)
    {
        if (cmdarg.args.empty())
        {
            g_logger.error("Logic error:cmdarg.args is empty!!");
            return NULL;
        }
    	redisReply* redis_reply = static_cast<redisReply*>(redisCommandArgv(m_redis_context,(int)cmdarg.argc,cmdarg.Args(),cmdarg.Arglen()));
    	if (redis_reply)
    	{
    		if (redis_reply->type == REDIS_REPLY_ERROR)
    		{
    			g_logger.error("redis response error:%s!",redis_reply->str);
    		}
    	}
    	else
    	{
    	    g_logger.error("Logic error:m_redis_reply is empty!%s!",cmdarg.args[0]);
    	}
    	return redis_reply;
    }
    void RedisSyncClient::FreeRedisObj(void *obj)
    {
        if (obj)
        {
            freeReplyObject(obj);
        }
    }

}//QCool end