#include "service/logic_base.h"
#include "service/service.h"
#include "logging/logging/zLogger.h"

void Logicbase::CloseLink( LinkKey key )
{
	FUN_EMPTY_WARN;
}

uint32_t Logicbase::ProcessMsg()
{
	Service::Instance().FetchMsg(cmd_queue_);
	uint32_t msg_count = UINT32(cmd_queue_.size());
	while (!cmd_queue_.empty())
	{
		stEvent eventmsg = cmd_queue_.front();	//取消息
		cmd_queue_.pop();
		ParseEvent(eventmsg); //处理所有事件         
	}
	return msg_count;
}

bool Logicbase::ParseEvent( stEvent& eventmsg )
{
	switch(eventmsg.event_)//事件
	{
	case EVT_ON_DATA://数据
		{
			ParseMsg(eventmsg);					//处理消息
			if (eventmsg.bofree)
			{
				FreePacketBuffer(eventmsg.pQueueMsg);
				eventmsg.pQueueMsg = NULL;
			}
		}
		break;
	case EVT_ON_CONNECT://连接建立
		{
			OnAddLink(eventmsg);
		}
		break;
	case EVT_ON_DISCONNECT://连接断开
		{
			OnDelLink(eventmsg);
		}
		break;
	case EVT_ON_CONNECT_FAIL://连接端连接失败
		{

		}
		break;
	case EVT_ON_LISTEN_FAIL://监听失败
		{
			g_logger.error("开启监听失败");
		}
		break;
	default:
		{
			g_logger.error("tcp事件类型错误:[%d]",eventmsg.event_);//error给release用
			//abort();//类型错误宕机
		}
	}
	return true;
}

bool Logicbase::ParseMsg( stEvent& eventmsg )
{
    if (eventmsg.pQueueMsg->cmdBuffer.value != 0x3fe)
    {
	    //g_logger.debug("parsemsg recive -------- client msg [0x%x]",eventmsg.pQueueMsg->cmdBuffer.value);
    }
	switch(eventmsg.pQueueMsg->cmdBuffer.value)
	{
	case 0x01fd://登陆
		{
			OnAddLogic(eventmsg);
		}
		break;
	case stCheckSignalCmd::_value://心跳包;
		{
			logic_manager_.OnCheckSignal(eventmsg);
		}
		break;
	default :
		{
			logic_manager_.DispachLogicMsg(eventmsg);
		}
	}
	return true;
}

bool Logicbase::AddConnect( MarkId mark,const std::string& ip,int32_t port )
{
	return Service::Instance().AddConnector(mark,ip,port);
}

bool Logicbase::AddListen( MarkId mark,const std::string& ip,int32_t port ,int type)
{
	return Service::Instance().AddListener(mark,ip,port,type);
}