#include "timermanager.h"
#include <cassert>
#include <algorithm>

namespace ashes {

TimerManager::TimerManager()
{
}

TimerManager::~TimerManager()
{
}

TimerHandle TimerManager::NewTimer1(
    TimerEvent1 event, 
    time::Msecs period,
    bool loop)
{
    UniqueTimer timer = AllocateTimer();
    TimerHandle handle = timer->Handle();
    timer->Init(std::move(event), period, loop);
    active_timers_.push_back(std::move(timer));
    return handle;
}

TimerHandle TimerManager::NewTimer2(
    TimerEvent2 event2,
    time::Msecs period,
    bool loop)
{
    TimerEvent1 event1 = [event2]() { event2(); return true; };
    return NewTimer1(event1, period, loop);
}

void TimerManager::SetTimer(
    TimerHandle handle,
    time::Msecs period,
    bool loop)
{
    if (UniqueTimer* timer = FindTimer(handle))
    {
        (*timer)->Modify(period, loop);
    }
}

void TimerManager::KillTimer(TimerHandle handle)
{
    if (UniqueTimer* timer = FindTimer(handle))
    {
        std::size_t index = timer - active_timers_.data();
        DeallocateTimer(std::move(*timer));
        active_timers_.erase(active_timers_.begin() + index);
    }
}

void TimerManager::InvalidateTimer(TimerHandle& handle)
{
    KillTimer(handle);
    handle = nullptr;
}

void TimerManager::Tick(time::Msecs timeslice)
{
    while (UniqueTimer* timer_ptr = FindEarliestTimer())
    {
        assert(*timer_ptr != nullptr);
        Timer& timer = **timer_ptr;
        
        // wait timer within timeslice.
        time::Msecs remaining_time = timer.RemainingTime();
        if (remaining_time > time::Msecs::zero())
        {
            if (timeslice < remaining_time) { break; }
            time::Sleep(remaining_time);
            timeslice -= remaining_time;
        }

        // trigger timer.
        if (!timer.Trigger())
        {
            KillTimer(timer.Handle());
        }
    }

    time::Sleep(timeslice);
}

TimerManager::UniqueTimer* TimerManager::FindTimer(TimerHandle handle)
{
    auto iter = std::find_if(active_timers_.begin(), active_timers_.end(),
        [handle](const UniqueTimer& timer) { return timer->Handle() == handle; });
    return iter == active_timers_.end() ? nullptr : &*iter;
}

TimerManager::UniqueTimer* TimerManager::FindEarliestTimer()
{
    auto iter = std::min_element(active_timers_.begin(), active_timers_.end(),
        [](const UniqueTimer& x, const UniqueTimer& y) {
            return x->RemainingTime() < y->RemainingTime(); });
    return iter == active_timers_.end() ? nullptr : &*iter;
}

TimerManager::UniqueTimer TimerManager::AllocateTimer()
{
    if (!dead_timers_.empty())
    {
        UniqueTimer timer = std::move(dead_timers_.back());
        dead_timers_.pop_back();
        return timer;
    }
    return std::make_unique<Timer>();
}

void TimerManager::DeallocateTimer(UniqueTimer timer)
{
    dead_timers_.push_back(std::move(timer));
}

void TimerManager::Timer::Init(
    TimerEvent1 event,
    time::Msecs period,
    bool loop)
{
    event_ = std::move(event);
    Modify(period, loop);
    RefreshLastTime();
}

TimerHandle TimerManager::Timer::Handle() const
{
    return this;
}

time::Msecs TimerManager::Timer::RemainingTime() const
{
    time::Msecs elapse = time::MsecsFrom(last_time_);
    return elapse >= period_ ? time::Msecs::zero() : period_ - elapse;
}

void TimerManager::Timer::Modify(time::Msecs period, bool loop)
{
    period_ = period;
    loop_ = loop;
}

void TimerManager::Timer::RefreshLastTime()
{
    last_time_ = time::Now();
}

bool TimerManager::Timer::Trigger()
{
    bool keep_loop = event_();
    RefreshLastTime();
    return keep_loop && loop_;
}

}