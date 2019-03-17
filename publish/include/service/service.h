#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <map>
#include <queue>
#include "utils/once_call.h"
#include "net/tcp/tcp_manager.h"
#include "msg_transform.h"
#include "define/common.h"

enum EventType
{
    EVT_ON_DATA = 0,//数据
    EVT_ON_CONNECT_FAIL = 1,//连接端连接失败
    EVT_ON_LISTEN_FAIL = 2,//监听失败
    EVT_ON_CONNECT = 3,//连接建立
    EVT_ON_DISCONNECT = 4,//连接断开
};

typedef uint32_t MarkId;

struct stEvent//事件和消息
{
    LinkKey id_;			//连接id
	MarkId mark_;
    int32_t event_;			//事件 EventType
    bool bofree;			//是否释放数据内存 默认为(true)删除
    stQueueMsg* pQueueMsg;	//当有数据时是非Null
    stEvent(){id_ = 0;mark_=0;event_=0;bofree=true;pQueueMsg=NULL;}
};

struct EndianInfo			//端对象信息
{
    int32_t objid_;			//uv底层对象id
	MarkId mark_;
    string ip_;
    int32_t port_;
	bool retry_;
};

typedef queue<stEvent> EventQueue;
typedef map<int32_t,EndianInfo>	 EndianMap;	//端对象信息列表

class Service: public QCOOL::Singleton<Service>
{
public:
    Service();
    ~Service();
public:
	/*-->[ 添加监听 ]*/
    bool AddListener( MarkId mark,const string& ip,int32_t port ,int type = 0);
	/*-->[ 添加连接 ]*/
    bool AddConnector( MarkId mark,const string& ip,int32_t port,bool retry = true);
	/*-->[ 获取收到的消息 ]*/
    void FetchMsg(EventQueue &cmd_queue);
	/*-->[ 发送消息:使用TcpMessage 上层来封包 因为上层可能有cache]*/
    bool SendMsg(TcpMessage& msg);			//
protected:
	/*-->[ 添加重连]*/
	bool AddReConnect(int32_t objid);
	/*-->[ 处理重连]*/
	void RunReConnect();
	/*-->[ 消息转换:将下层消息转换成上层可用消息]*/
    bool TransMsg(TcpMessage& tcpmsg,stEvent& eventandmsg);
private:
    TcpMessageQueue msg_queue_;
	EndianMap endian_map_;					//端对象列表
	EndianMap reconnect_map_;				//重连列表
	time_t next_reconn_time_;
};

#endif //_SERVICE_H_