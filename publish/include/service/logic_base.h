#ifndef _LOGICBASE_H__
#define _LOGICBASE_H__

#include "service/service.h"
#include "logic_obj.h"
#include "server_status.h"
#include <string>

//方法未实现警告
#define FUN_EMPTY_WARN g_logger.error("调用提醒：该函数暂时未实现 (%s:%s)",__FILE__,__FUNCTION__);

struct CommonObj
{
	LinkKey key_;//连接id
};

typedef map<LinkKey,CommonObj> CommonObjsMap;	//连接后建立的通用对象

class Logicbase
{
public:
	/*-->[初始化,读取配置等 ]*/
	virtual bool Init(std::string & strServerId) = 0;
	/*-->[启动,开启监听和连接 ]*/
	virtual bool Start()= 0;
	/*-->[运行,循环处理消息和事件 ]*/
	virtual void Run() = 0;
	/*-->[停止服务器 ]*/
	virtual void Stop() = 0;
	/*-->[主动关闭某个连接 ]*/
	void CloseLink(LinkKey key);			//zc：todo 暂时不实现
protected:
	/*-->[ 消息循环，返回一次循环处理的消息条数]*/
	uint32_t ProcessMsg();	
	/*-->[ 处理连接事件 ]*/
	bool ParseEvent(stEvent& eventmsg);
	/*-->[ 处理消息]*/
	bool ParseMsg(stEvent& eventmsg);
protected:
	/*-->[ 添加连接端口 ]*/
	bool AddConnect(MarkId mark,const std::string& ip,int32_t port);
	/*-->[ 添加监听端口 ]*/
	bool AddListen(MarkId mark,const std::string& ip,int32_t port,int type = 0);
	/*-->[ 连接建立回调 ]*/
	virtual void OnAddLink(stEvent& eventmsg){};
	/*-->[ 连接断开回调 ]*/
	virtual void OnDelLink(stEvent& eventmsg){};
	/*-->[ 添加逻辑对象回调 ]*/
	virtual void OnAddLogic(stEvent& eventmsg) = 0;
protected:
	std::string 		m_strServerId;					//服务器id
	ServerStatusCtrl	status_;						//服务器状态
	EventQueue			cmd_queue_;						//消息事件队列(只在逻辑线程中调用所以不用加锁)
	LogicObjManager		logic_manager_;					//逻辑对象管理器
};

#endif //_LOGICBASE_H__