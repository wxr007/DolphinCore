#include "service/logic_obj.h"
#include "logging/logging/zLogger.h"

const int32_t SERVER_CACHE_SEND_SIZE = 16*1024;
const int32_t SERVER_CACHE_SIZE = 16*1024 + 4*1024;

LogicObj::LogicObj( LinkKey key,MarkId mark )
{
	link_ = key;
	mark_ = mark;
	cache_.reserve(SERVER_CACHE_SIZE);//[cache]
}

LogicObj::~LogicObj()
{
}
//[cache]
bool LogicObj::SendCmdDelay( void* pbuf, uint32_t nsize,int zliblvl )
{
	if (MsgTrans::data_encode_to_cache(pbuf,nsize,zliblvl,cache_))
	{
		bool ret = true;
		if (cache_.size() >= SERVER_CACHE_SEND_SIZE)
		{
			ret = SendCache();
		}
		return ret;
	}
	return false;
}
//[cache]
bool LogicObj::SendCmd( void* pbuf, uint32_t nsize,int zliblvl )
{
	TcpMessage tcp_message;
	tcp_message.key_ = link_;
	MsgTrans::encodeMessage(pbuf,nsize,zliblvl,tcp_message);
	return Service::Instance().SendMsg(tcp_message);
}
//[cache]
bool LogicObj::SendCache()
{
    bool ret = true;
    if (!cache_.empty())
    {
        TcpMessage tcp_message;

        tcp_message.key_ = link_;
        tcp_message.data_new(cache_.size());
        memcpy(tcp_message.data_, cache_.data(), cache_.size());
        ret = Service::Instance().SendMsg(tcp_message);
        cache_.clear();
    }
    return ret;
}

bool LogicObjManager::AddLogic( LogicObj_ptr obj )
{
	if (obj)
	{
		return logic_obj_map_.insert(LogicObjMap::value_type(obj->link_,obj)).second;
	}
	return false;
}

void LogicObjManager::DelLogic( LinkKey link )
{
	logic_obj_map_.erase(link);
}

LogicObj_ptr LogicObjManager::find( LinkKey link )
{
	LogicObjMap::iterator it = logic_obj_map_.find(link);
	if (it != logic_obj_map_.end())
	{
		return it->second;
	}
	return LogicObj_ptr();
}

void LogicObjManager::DispachLogicMsg( stEvent& eventmsg )
{
	LogicObjMap::iterator it = logic_obj_map_.find(eventmsg.id_);
	if (it != logic_obj_map_.end())
	{
		//消息分发
		stQueueMsg*& qmsg = eventmsg.pQueueMsg;
		if (it->second)
		{
			it->second->MsgParse(&qmsg->cmdBuffer,qmsg->cmdsize,eventmsg.bofree,qmsg);
		}
	}
	else
	{
		g_logger.error("分发消息时未找到相应的逻辑对象[0x%"PRIx64"]",eventmsg.id_);
	}
}

void LogicObjManager::OnCheckSignal( stEvent& eventmsg )
{
	stCheckSignalCmd *pCheckSingal = (stCheckSignalCmd *)(&eventmsg.pQueueMsg->cmdBuffer);
	if (pCheckSingal->isneedACK)
	{
		LogicObjMap::iterator it = logic_obj_map_.find(eventmsg.id_);
		if (it != logic_obj_map_.end())
		{
			stCheckSignalCmd cmd(pCheckSingal);
			if (it->second)
			{
				it->second->SendCmd( &cmd, sizeof(stCheckSignalCmd));
			}
		}
	}
}

void LogicObjManager::SendCmd( LinkKey id,void* pbuf, uint32_t nsize,int zliblvl,bool delay )
{
	LogicObjMap::iterator it = logic_obj_map_.find(id);
	if (it != logic_obj_map_.end())
	{
		if (it->second)
		{
			if (delay)
			{
				it->second->SendCmdDelay(pbuf,nsize,zliblvl);
			}
			else
			{
				it->second->SendCmd(pbuf,nsize,zliblvl);
			}
		}
	}
	else
	{
		g_logger.error("发送消息时未找到相应的逻辑对象[%d]",id);
	}
}

void LogicObjManager::FlushCache()
{
	for(LogicObjMap::iterator it = logic_obj_map_.begin();it != logic_obj_map_.end();it++)
	{
		if (it->second)
		{
			it->second->SendCache();
		}
	}
}
