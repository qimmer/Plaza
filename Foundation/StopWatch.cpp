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

static void UpdateStopWatch(Entity stopWatch, StopWatch& data) {
    if(!data.StopWatchRunning) return;

#ifdef WIN32
    LARGE_INTEGER currentTime;
    auto freq = GetFrequency();
    QueryPerformanceCounter(&currentTime);

    double deltaTime = (double)(currentTime.QuadPart - data.lastTime) / freq.QuadPart;

    data.lastTime = (u64)currentTime.QuadPart;
#else
    timespec now;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);

        auto delta = diff(data->lastTime, now);
        double deltaTime = (double)delta.tv_sec + ((double)delta.tv_nsec / 1000000000.0);
#endif

    data.StopWatchElapsedSeconds += deltaTime;
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    StopWatch data;
    for_entity_data(stopWatch, ComponentOf_StopWatch(), &data) {
        UpdateStopWatch(stopWatch, data);
        SetStopWatch(stopWatch, data);
    }
}

static void OnStopWatchChanged(Entity stopWatch, const StopWatch& oldData, const StopWatch& newData) {
    auto data = GetStopWatch(stopWatch);

    if(!oldData.StopWatchRunning && newData.StopWatchRunning) {
#ifdef WIN32
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        data.lastTime = currentTime.QuadPart;
#else
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &data->lastTime);
#endif
    } else if(oldData.StopWatchRunning && !newData.StopWatchRunning) {
        // Make sure to include time from last update and until now when stopped!
        data.StopWatchRunning = true;
        UpdateStopWatch(stopWatch, data);
        data.StopWatchRunning = false;
        SetStopWatch(stopWatch, data);
    }

    if(newData.StopWatchElapsedSeconds != oldData.StopWatchElapsedSeconds) {
#ifdef WIN32
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        data.lastTime = currentTime.QuadPart;
#else
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &data->lastTime);
#endif
    }
}

BeginUnit(StopWatch)
    BeginComponent(StopWatch)
        RegisterProperty(bool, StopWatchRunning)
        RegisterProperty(double, StopWatchElapsedSeconds)
    EndComponent()

    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_Input)
    RegisterSystem(OnStopWatchChanged, ComponentOf_StopWatch())
EndUnit()