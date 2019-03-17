
#include "net/tcp/uv_timer.h"

const int32_t HEARTBEAT_INTERVAL = 1000;//1sec;
#if 0
HeartBeatTimer::HeartBeatTimer(int32_t cycle)
:cycle_(cycle), cycle_pos_(0)
{
    timeout_ = HEARTBEAT_INTERVAL;
    next_timeout_ = HEARTBEAT_INTERVAL;
    cb_ = QCOOL::bind(&HeartBeatTimer::OnTimer,this,_1);
}

size_t HeartBeatTimer::GetCycleRef(int32_t pos)
{
    return pos%cycle_.size();
}

void HeartBeatTimer::Register(LinkKey k, HBTimerCallback&cb)
{
    if (!cb)
    {//如果函数为空,直接ruturn.
        return;
    }
    HBTimerMap::iterator pos = heartbeat_timers_.find(k);
    assert(pos == heartbeat_timers_.end());
    HeartBeatNode &node = heartbeat_timers_[k];
    node.cb_ = cb;
    node.net_id_ = k;
    node.ref_ = GetCycleRef(cycle_pos_);

    assert(node.ref_ < cycle_.size());
    CycleNode &cnode = cycle_.at(node.ref_);
    assert(cnode.find(k) == cnode.end());
    cnode.insert(k);
}

void HeartBeatTimer::UnRegister(LinkKey k)
{
    HBTimerMap::iterator pos = heartbeat_timers_.find(k);
    assert(pos != heartbeat_timers_.end());
    HeartBeatNode &node = pos->second;

    assert(node.ref_ < cycle_.size());
    CycleNode &cnode = cycle_.at(node.ref_);
    assert(cnode.find(k) != cnode.end());
    cnode.erase(k);
}

void HeartBeatTimer::OnTimer(TimerId id)
{
    size_t ref = GetCycleRef(cycle_pos_);
    ++cycle_pos_;
    assert(ref < cycle_.size());
    CycleNode &node = cycle_.at(ref);
    //遍历;
    CycleNode::const_iterator cpos = node.begin();
    CycleNode::const_iterator cend = node.end();
    HBTimerMap::iterator mit;
    while (cpos != cend)
    {
        mit = heartbeat_timers_.find(*cpos);
        assert(mit != heartbeat_timers_.end());
        mit->second.cb_(*cpos,id);
        ++cpos;
    }
}

#endif















