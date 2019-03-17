
#ifndef _NET_SYS_H__
#define _NET_SYS_H__

#include "net/tcp/tcp_manager.h"

inline void default_update_func(LinkKey )
{

}

inline void NetStart()
{
    Libuv::Instance().Start();
    LinkUpdateFunc fuc(default_update_func);
    //TcpManager::Instance().InitSelf(fuc,fuc);
    //TcpManager::Instance().InitSelf(std::move(default), std::move(default));
}

inline void NetStop()
{
    //TcpManager::Instance().Stop();
    Libuv::Instance().Stop();
}

#endif //_NET_SYS_H__
