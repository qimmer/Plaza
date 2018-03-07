//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Foundation/AppLoop.h>
#include <Core/Debug.h>

#ifdef WIN32
#include <windows.h>
#include <Core/String.h>

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

static double deltaTime = 0.0;

DefineEvent(AppUpdate, AppUpdateHandler)

void AppUpdate() {
    auto startTime = GetTimeSinceStart();
    FireEvent(AppUpdate, deltaTime);
    deltaTime = GetTimeSinceStart() - startTime;

    // Free temporary strings that have accumulated during this frame
    FreeTempStrings();
}

double GetDeltaTime() {
    return deltaTime;
}

