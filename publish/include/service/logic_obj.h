#ifndef _LOGICOBJ_H__
#define _LOGICOBJ_H__

#include "service/service.h"
#include "utils/smart_ptr.h"

enum SignalState
{
    SS_NONE = 0,
    SS_WAIT_ACK = 1,	//发送心跳包;等待心跳ack;
    SS_ACK_RECVED = 2,	//收到ack心跳;
    SS_ERROR = 3,		//等心跳ace超时;
};
class LogicObjManager;

//逻辑对象基础类
class LogicObj
{
public:
    LogicObj(LinkKey key,MarkId mark);
    virtual ~LogicObj();
	/*-->[ 消息处理 ]*/
    virtual bool MsgParse(stBaseCmd* pcmd, uint32_t ncmdlen,bool& freecmd,stQueueMsg* msgbuffer) = 0;
	/*-->[ 直接发送消息 ]*/
    virtual bool SendCmd(void* pbuf, uint32_t nsize,int zliblvl = Z_DEFAULT_COMPRESSION);
	/*-->[ 延迟发送消息:使用cache]*/
    bool SendCmdDelay( void* pbuf, uint32_t nsize,int zliblvl = Z_DEFAULT_COMPRESSION);
	/*-->[ 发送缓冲:将cache中的数据一次发光 ]*/
    bool SendCache();
protected:
	friend class LogicObjManager;
    LinkKey link_;//连接id
	MarkId mark_;	//mark
    string cache_;//[cache]
public:
    int32_t signal_state_;
};

typedef QCOOL::shared_ptr<LogicObj> LogicObj_ptr;
typedef map<LinkKey,LogicObj_ptr> LogicObjMap;

class LogicObjManager
{
public:
	/*-->[ 添加逻辑对象 ]*/
	bool AddLogic(LogicObj_ptr obj);
	/*-->[ 删除逻辑对象 ]*/ 
	void DelLogic(LinkKey link);
	/*-->[ 查找逻辑对象 ]*/ 
	LogicObj_ptr find(LinkKey link);
	/*-->[ 分发消息]*/
	void DispachLogicMsg(stEvent& eventmsg);
	/*-->[ 收到心跳 ]*/ 
	void OnCheckSignal(stEvent& eventmsg);
	/*-->[ 发送消息 ]*/
	void SendCmd(LinkKey id,void* pbuf, uint32_t nsize,int zliblvl = Z_DEFAULT_COMPRESSION,bool delay = true);
	/*-->[ 发送缓冲 ]*/ 
	void FlushCache();
private:
	LogicObjMap logic_obj_map_;
};

#endif // _LOGICOBJ_H__