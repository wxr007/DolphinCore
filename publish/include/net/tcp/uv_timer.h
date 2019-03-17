#ifndef _UV_TIMER_H__
#define _UV_TIMER_H__

#include "utils/function.h"
#include "utils/smart_ptr.h"
#include "net/tcp/uv_ex.h"

const int32_t TIMER_PRECISION = 100;//计量精度为100毫秒.
typedef int64_t TimerId;
typedef QCOOL::function<void(TimerId)> TimerCallback;
struct TimerBase
{
    TimerId timer_id_;
    int32_t timeout_;
    int32_t next_timeout_;
    //int32_t precision_;//计量精度,单位为毫秒(ms);
    TimerCallback cb_;//如果是类成员函数的回调,那么请自己保证类对象的生命周期.
    virtual void OnTimer(TimerId) = 0;
};

//typedef shared_ptr<TimerBase> TimerBaseSPtr;


//HeartBeatTimer:注册心跳一个回调函数,定时回调该函数;
typedef QCOOL::function<void(LinkKey, TimerId)> HeatBeatTimerCallback;
typedef HeatBeatTimerCallback HBTimerCallback;

struct HeartBeatNode
{
    LinkKey net_id_;
    HBTimerCallback cb_;
    size_t ref_;
};

class HeartBeatTimer : public TimerBase
{
public :
    HeartBeatTimer(int32_t cycle/*second*/);
    void Register(LinkKey k, HBTimerCallback&);
    void UnRegister(LinkKey k);
    void OnTimer(TimerId );
    size_t GetCycleRef(int32_t pos);
private :
    typedef QCOOL::unordered_map<LinkKey, HeartBeatNode> HBTimerMap;
    HBTimerMap heartbeat_timers_;
    //
    typedef QCOOL::unordered_set<LinkKey> CycleNode;
    std::vector<CycleNode> cycle_;
    //const int32_t cycle_size_;
    int32_t cycle_pos_;
};




#endif //_UV_TIMER_H__
