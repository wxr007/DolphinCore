#include <string>
#include "service/server_status.h"
#include "logging/logging/zLogger.h"


ServerStatus ServerStatusCtrl::GetStatus()
{
	return server_status_;
}

void ServerStatusCtrl::SetStatus( ServerStatus status,StatusStep step )
{
	server_status_ =  status;
	std::string step_str;
	switch(step)
	{
	case STATUS_START:
		{
			step_str = "开始";
		}
		break;
	case STATUS_SUCCESS:
		{
			step_str = "成功";
		}
		break;
	case STATUS_FAILD:
		{
			step_str = "失败";
		}
		break;
    default :
        {
            g_logger.forceLog(zLogger::zINFO, "Unknow step[%d]...",step);
            assert(0);
            break;
        }
	}
	switch (server_status_)
	{
	case SVR_START:
		{
			g_logger.forceLog(zLogger::zINFO, "程序启动[%s]...",step_str.c_str());
		}
		break;
	case SVR_CONFIG_INIT:
		{
			g_logger.forceLog(zLogger::zINFO, "程序配置初始化[%s]...",step_str.c_str());
		}
		break;
	case SVR_CREAT_TCP:
		{
			g_logger.forceLog(zLogger::zINFO, "程序创建网络对象[%s]...",step_str.c_str());
		}
		break;
	case SVR_RUN:
		{
			g_logger.forceLog(zLogger::zINFO, "程序运行[%s]...",step_str.c_str());
		}
		break;
	case SVR_STOP:
		{
			g_logger.forceLog(zLogger::zINFO, "程序关闭[%s]...",step_str.c_str());
		}
		break;
    case SVR_MODEL_CFG:
        {
            g_logger.forceLog(zLogger::zINFO, "模块初始化[%s]...",step_str.c_str());
        }
        break;
    default:
        {
            g_logger.forceLog(zLogger::zINFO, "Unknow server_status_[%d]...",server_status_);
            assert(0);
            break;
        }
	}

}
