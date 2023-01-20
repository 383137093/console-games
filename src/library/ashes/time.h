#ifndef ASHES_TIME_H
#define ASHES_TIME_H

#include <chrono>
#include <thread>

namespace ashes { namespace time {

typedef std::chrono::steady_clock Clock;
typedef Clock::time_point         TimePoint;
typedef std::chrono::milliseconds Msecs;

inline TimePoint Now()
{
    return Clock::now();
}

inline Msecs MsecsBetween(TimePoint from, TimePoint to)
{
    return std::chrono::duration_cast<Msecs>(to - from);
}

inline Msecs MsecsFrom(TimePoint t)
{
    return MsecsBetween(t, Now());
}

inline Msecs MsecsTo(TimePoint t)
{
    return MsecsBetween(Now(), t);
}

inline void Sleep(Msecs msecs)
{
    std::this_thread::sleep_for(msecs);
}

}}

#endif