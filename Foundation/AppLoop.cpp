//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Foundation/AppLoop.h>
#include <Core/Debug.h>

#include <Core/String.h>

#ifdef WIN32
#undef GetHandle
#include <windows.h>

static double PCFreq = 0.0;

double GetTimeSinceStart()
{
    LARGE_INTEGER li;

    if(PCFreq == 0.0) {
        if(!QueryPerformanceFrequency(&li)) {
            Log(LogChannel_Core, LogSeverity_Fatal, "QueryPerformanceFrequency failed!");
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

double GetTimeSinceStart()
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
static double deltaTime = 0.0;

DefineEvent(AppUpdate, AppUpdateHandler)

void AppUpdate() {
    auto startTime = GetTimeSinceStart();
    FireEvent(AppUpdate, deltaTime);
    deltaTime = GetTimeSinceStart() - startTime;
}

double GetDeltaTime() {
    return deltaTime;
}

