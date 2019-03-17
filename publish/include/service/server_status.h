#ifndef _SERVERSTATUS_H_
#define _SERVERSTATUS_H_

enum ServerStatus
{
	SVR_START		= 0,			//程序启动
	SVR_CONFIG_INIT = 1,			//程序配置初始化
	SVR_MODEL_CFG	= 2,			//程序配置初始化
	SVR_CREAT_TCP	= 3,			//程序创建网络对象
	SVR_RUN			= 4,			//服务器运行
	SVR_STOP		= 5,			//服务器关闭
};
enum StatusStep
{
	STATUS_START = 0,		//开始
	STATUS_SUCCESS = 1,		//成功
	STATUS_FAILD = 2,		//失败
};

class ServerStatusCtrl
{
public:
	/*-->[ 获取服务器状态 ]*/
	ServerStatus GetStatus();
	/*-->[ 设置服务器状态 ]*/
	void SetStatus(ServerStatus status,StatusStep step);
private:
	ServerStatus server_status_;	//服务器状态标记
};

#endif //_SERVERSTATUS_H_