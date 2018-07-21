//
// Created by Kim on 06/07/2018.
//

#include <Foundation/StopWatch.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>

#ifdef WIN32
#ifdef WIN32
#undef GetHandle
#include <Windows.h>
#endif

struct StopWatch {
    bool StopWatchRunning;
    double StopWatchElapsedSeconds;

#ifdef WIN32
    LARGE_INTEGER lastTime;
#else
    timeval startTime;
#endif
};

#undef CreateEvent

static LARGE_INTEGER GetFrequency() {
    static LARGE_INTEGER PCFreq = {0, 0};
    if(PCFreq.QuadPart == 0) {
        if(!QueryPerformanceFrequency(&PCFreq)) {
            Log(0, LogSeverity_Fatal, "QueryPerformanceFrequency failed!");
        }
    }

    return PCFreq;
}

LocalFunction(OnAppLoopFrameChanged, void, Entity appLoop, u64 oldFrame, u64 newFrame) {
    LARGE_INTEGER currentTime;
    auto freq = GetFrequency();

    for_entity(entity, data, StopWatch) {
        if(!data->StopWatchRunning) continue;

        QueryPerformanceCounter(&currentTime);

        double deltaTime = (double)(currentTime.QuadPart - data->lastTime.QuadPart) / freq.QuadPart;
        SetStopWatchElapsedSeconds(entity, data->StopWatchElapsedSeconds + deltaTime);
        data->lastTime = currentTime;
    }
}

LocalFunction(OnStopWatchRunningChanged, void, Entity stopWatch, bool oldValue, bool newValue) {
    if(newValue) {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        auto data = GetStopWatchData(stopWatch);
        data->lastTime = currentTime;
    }
}

LocalFunction(OnStopWatchElapsedSecondsChanged, void, Entity stopWatch, double oldValue, double newValue) {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    auto data = GetStopWatchData(stopWatch);
    data->lastTime = currentTime;
}

#endif

#ifdef __APPLE__
#include <mach/mach_time.h>

static double GetTimeSinceStart()
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


BeginUnit(StopWatch)
    BeginComponent(StopWatch)
        RegisterProperty(bool, StopWatchRunning)
        RegisterProperty(double, StopWatchElapsedSeconds)
    EndComponent()

    RegisterSubscription(AppLoopFrameChanged, OnAppLoopFrameChanged, 0)
    RegisterSubscription(StopWatchRunningChanged, OnStopWatchRunningChanged, 0)
    RegisterSubscription(StopWatchElapsedSecondsChanged, OnStopWatchElapsedSecondsChanged, 0)
EndUnit()