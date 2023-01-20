#ifndef ASHES_TIMERMANAGER_H
#define ASHES_TIMERMANAGER_H

#include <memory>
#include <vector>
#include <functional>
#include "time.h"

namespace ashes {

typedef const void*           TimerHandle;  // use nullptr as a null timer handle.
typedef std::function<bool()> TimerEvent1;  // return false indicate kill the timer.
typedef std::function<void()> TimerEvent2;  // return value is ignored.

class TimerManager
{
public:

    TimerManager();
    TimerManager(const TimerManager&) = delete;
    ~TimerManager();
    TimerManager& operator = (const TimerManager&) = delete;

    TimerHandle NewTimer1(TimerEvent1 event, time::Msecs period, bool loop);
    TimerHandle NewTimer2(TimerEvent2 event, time::Msecs period, bool loop);
    void SetTimer(TimerHandle handle, time::Msecs period, bool loop);
    void KillTimer(TimerHandle handle);
    void InvalidateTimer(TimerHandle& handle);

    void Tick(time::Msecs timeslice);

private:

    class Timer
    {
    public:
        void Init(TimerEvent1 event, time::Msecs period, bool loop);
        TimerHandle Handle() const;
        time::Msecs RemainingTime() const;
        void Modify(time::Msecs period, bool loop);
        void RefreshLastTime();
        bool Trigger();
    private:
        TimerEvent1     event_;
        time::Msecs     period_;
        bool            loop_;
        time::TimePoint last_time_;
    };

private:

    typedef std::unique_ptr<Timer>   UniqueTimer;
    typedef std::vector<UniqueTimer> TimerVector;

    UniqueTimer* FindTimer(TimerHandle handle);
    UniqueTimer* FindEarliestTimer();

    UniqueTimer AllocateTimer();
    void DeallocateTimer(UniqueTimer timer);

    TimerVector active_timers_;
    TimerVector dead_timers_;
};

}

#endif