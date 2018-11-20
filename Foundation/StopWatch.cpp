//
// Created by Kim on 06/07/2018.
//

#include <Foundation/StopWatch.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>



#ifdef WIN32
#undef GetHandle
#undef Enum
#include <Windows.h>
#endif

#undef CreateEvent

struct StopWatch {
    bool StopWatchRunning;
    double StopWatchElapsedSeconds;
    Entity StopWatchUpdateLoop;

#ifdef WIN32
    LARGE_INTEGER lastTime;
#else
    timespec lastTime;
#endif
};

#ifdef WIN32
static LARGE_INTEGER GetFrequency() {
    static LARGE_INTEGER PCFreq = {0, 0};
    if(PCFreq.QuadPart == 0) {
        if(!QueryPerformanceFrequency(&PCFreq)) {
            Log(0, LogSeverity_Fatal, "QueryPerformanceFrequency failed!");
        }
    }

    return PCFreq;
}
#else
#include <time.h>

static timespec diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

#endif

LocalFunction(OnAppLoopFrameChanged, void, Entity appLoop, u64 oldFrame, u64 newFrame) {
    auto stopWatch = GetOwner(appLoop);
    auto data = GetStopWatchData(stopWatch);

    if(data) {
        if(!data->StopWatchRunning) return;

#ifdef WIN32
        LARGE_INTEGER currentTime;
        auto freq = GetFrequency();
        QueryPerformanceCounter(&currentTime);

        double deltaTime = (double)(currentTime.QuadPart - data->lastTime.QuadPart) / freq.QuadPart;

        data->lastTime = currentTime;
#else
        timespec now;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);

        auto delta = diff(data->lastTime, now);
        double deltaTime = (double)delta.tv_sec + ((double)delta.tv_nsec / 1000000000.0);
#endif

        SetStopWatchElapsedSeconds(stopWatch, data->StopWatchElapsedSeconds + deltaTime);
    }
}

LocalFunction(OnStopWatchRunningChanged, void, Entity stopWatch, bool oldValue, bool newValue) {
    if(newValue) {
        auto data = GetStopWatchData(stopWatch);

#ifdef WIN32
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        data->lastTime = currentTime;
#else
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &data->lastTime);
#endif
    }
}

LocalFunction(OnStopWatchElapsedSecondsChanged, void, Entity stopWatch, double oldValue, double newValue) {
    auto data = GetStopWatchData(stopWatch);

#ifdef WIN32
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    data->lastTime = currentTime;
#else
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &data->lastTime);
#endif
}

BeginUnit(StopWatch)
    BeginComponent(StopWatch)
        RegisterProperty(bool, StopWatchRunning)
        RegisterProperty(double, StopWatchElapsedSeconds)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnAppLoopFrameChanged, AppLoopOf_StopWatchUpdate())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_StopWatchRunning()), OnStopWatchRunningChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_StopWatchElapsedSeconds()), OnStopWatchElapsedSecondsChanged, 0)

    SetAppLoopOrder(AppLoopOf_StopWatchUpdate(), AppLoopOrder_Input);
EndUnit()