//
// Created by Kim on 05/07/2018.
//

#include <Foundation/Timer.h>
#include <Core/Debug.h>

struct Timer {
    double TimerInterval;
    bool TimerStarted, TimerRepeat;
};

BeginUnit(Timer)
    BeginComponent(Timer)
        RegisterProperty(double, TimerInterval)
        RegisterProperty(bool, TimerStarted)
        RegisterProperty(bool, TimerRepeat)
    EndComponent()

    RegisterFunction(GetTimeSinceStart)
EndUnit()


#ifdef WIN32
#undef GetHandle
#include <windows.h>

#undef CreateEvent
static double PCFreq = 0.0;

API_EXPORT double GetTimeSinceStart()
{
    LARGE_INTEGER li;

    if(PCFreq == 0.0) {
        if(!QueryPerformanceFrequency(&li)) {
            Log(0, LogSeverity_Fatal, "QueryPerformanceFrequency failed!");
            return 0.0;
        }

        PCFreq = double(li.QuadPart);
    }

    QueryPerformanceCounter(&li);
    return double(li.QuadPart)/PCFreq;
}
#endif

#ifdef __APPLE__
#include <mach/mach_time.h>

API_EXPORT double GetTimeSinceStart()
{
    const u64 kOneMillion = 1000 * 1000 * 1000;
    static mach_timebase_info_data_t s_timebase_info {0, 0};

    if(!s_timebase_info.denom) {
        mach_timebase_info(&s_timebase_info);
    }

    // mach_absolute_time() returns billionth of seconds,
    // so divide by one million to get milliseconds
    return (double)(mach_absolute_time() * s_timebase_info.numer) / (kOneMillion * s_timebase_info.denom);
}
#endif
