#include "service/service.h"
#include "net/net_sys.h"
#include "logging/logging/zLogger.h"
#include "define/common.h"

int32_t GetObjType(int32_t objid)
{
	return (objid&(~TCPOBJ_ID_MASK))&MASK32BIT;
}

Service::Service()
{
    NetStart();
}

Service::~Service()
{
    NetStop();
}

bool Service::AddListener( MarkId mark,const string& ip,int32_t port ,int type)
{
	int32_t objid = TcpManager::Instance().AddServer(ip,port,"",type);
	EndianInfo info;
	info.objid_ = objid;
	info.mark_ = mark;
	info.ip_ = ip;
	info.port_ = port;
	if (endian_map_.insert(EndianMap::value_type(info.objid_,info)).second)
	{
		return true;
	}
	return false;
}

bool Service::AddConnector( MarkId mark,const string& ip,int32_t port,bool retry)
{
	int32_t objid = TcpManager::Instance().AddClient(ip,port,"");
	EndianInfo info;
	info.objid_ = objid;
	info.mark_ = mark;
	info.ip_ = ip;
	info.port_ = port;
	info.retry_ = retry;
	if (endian_map_.insert(EndianMap::value_type(info.objid_,info)).second)
	{
		return true;
	}
	return false;
}

void Service::FetchMsg(EventQueue &cmd_queue)
{
	stEvent eventandmsg;
	TcpManager::Instance().FetchRecvQueue(msg_queue_);

	for (TcpMessageQueue::iterator it = msg_queue_.begin();it != msg_queue_.end();it++)
	{
		TcpMessage& msg = *(it);
		if (TransMsg(msg,eventandmsg) == false)//把下层消息转换成上层消息
		{
			msg.data_free();
			continue;
		}	
		msg.data_free();
		cmd_queue.push(eventandmsg);	//压入队列上层取 (如有想用回调也可以在这里直接回调)
	}
	msg_queue_.clear();
	RunReConnect();							//运行一下重连
}

bool Service::SendMsg( TcpMessage& msg )
{
	if (TcpManager::Instance().SendData(msg) == 0)
	{
		return true;
	}
	return false;
}

bool Service::TransMsg( TcpMessage& tcpmsg,stEvent& eventandmsg )
{
    eventandmsg.id_ = tcpmsg.key_;
    int32_t objid = GETOBJECTID(tcpmsg.key_);
	EndianMap::iterator it = endian_map_.find(objid);
	if (it != endian_map_.end())
	{
		eventandmsg.mark_ = it->second.mark_;
    }
	else
	{
		g_logger.error("底层产生消息时，上层无法找到该objid (%d)",objid);
		return false;
	}
    switch(tcpmsg.type_)
    {
    case TM_TYPE_NET_DATA://数据
        {
            eventandmsg.event_ = EVT_ON_DATA;
			if ( MsgTrans::decodeMessage(eventandmsg.pQueueMsg,tcpmsg) == false)//转包
			{
				g_logger.error("逻辑层解包失败:[key:0x%"PRIx64",cmd:0x%x,len:%u]",eventandmsg.id_,eventandmsg.pQueueMsg->cmdBuffer.value,eventandmsg.pQueueMsg->cmdsize);
				return false;
			}
        }
        break;
    case TM_TYPE_CLI_START_FAILED://连接端连接失败
    case TM_TYPE_CLI_ERROR:
    case TM_TYPE_CLI_CONNECT_FAILED:
        {
            eventandmsg.event_ = EVT_ON_CONNECT_FAIL;
			AddReConnect(objid);
        }
        break;
    case TM_TYPE_SVR_START_FAILED://监听失败
        {
            eventandmsg.event_ = EVT_ON_LISTEN_FAIL;
        }
        break;
    case TM_TYPE_CLI_CONNECT_OK://连接建立
    case TM_TYPE_SVR_ACCEPT_OK:
        {
            eventandmsg.event_ = EVT_ON_CONNECT;
        }
        break;
    case TM_TYPE_LINK_ERROR://连接断开
    case TM_TYPE_SYSTEM_STOP:
        {
            eventandmsg.event_ = EVT_ON_DISCONNECT;
			AddReConnect(objid);
        }
        break;
    case TM_TYPE_SVR_ACCEPT_FAILED:
        {//accept失败,可以忽略;
        }
        break;
    case TM_TYPE_SVR_ERROR:
        {//该情况,一般是不可能发生的;
        }
        break;
    default:
        {
            //error给release用
            //abort();//类型错误直接宕机
        }
    }
	return true;
}

bool Service::AddReConnect( int32_t objid )
{
	if (GetObjType(objid) == TCPOBJ_CLIENT)	//连接端才需要重连
	{
		EndianMap::iterator it = endian_map_.find(objid);
		if (it != endian_map_.end())
		{
			EndianInfo& info = (EndianInfo&)it->second;
			if (info.retry_)
			{
				if (reconnect_map_.insert(EndianMap::value_type(objid,info)).second)
				{
					endian_map_.erase(it);
					return true;
				}
			}
		}
	}
	return false;
}

void Service::RunReConnect()
{
	time_t cur_run_time = time(NULL);
	if (cur_run_time > next_reconn_time_)//重连间隔时间
	{
		next_reconn_time_ = cur_run_time + 2;
		for (EndianMap::iterator it = reconnect_map_.begin();it != reconnect_map_.end();it++)
		{
			AddConnector(it->second.mark_,it->second.ip_,it->second.port_,it->second.retry_);
		}
		reconnect_map_.clear();
	}
}

