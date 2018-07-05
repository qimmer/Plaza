//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Foundation/AppLoop.h>
#include <Core/Debug.h>

#include <Core/String.h>

static const double FixedUpdateInterval = 1.0 / 30.0;
static double ScreenScanInterval = 1.0 / 60.0;


static double deltaTime = 0.0, timeSinceLastFixedUpdate = 0.0, timeSinceLastScreenScanUpdate = 0.0;

DefineEvent(AppUpdate)
DefineEvent(AppFixedUpdate)
DefineEvent(AppScreenScanUpdate)

API_EXPORT void SetScreenScanInterval(double interval) {
    ScreenScanInterval = interval;
}

API_EXPORT double GetScreenScanInterval() {
    return ScreenScanInterval;
}

API_EXPORT void AppUpdate() {
    auto startTime = GetTimeSinceStart();
    FireNativeEvent(AppUpdate, 0, deltaTime);
    deltaTime = GetTimeSinceStart() - startTime;

    timeSinceLastFixedUpdate += deltaTime;
    timeSinceLastScreenScanUpdate += deltaTime;

    if(timeSinceLastFixedUpdate >= FixedUpdateInterval) {
        auto deltaFixedTime = timeSinceLastFixedUpdate;
        timeSinceLastFixedUpdate = 0.0;

        FireNativeEvent(AppFixedUpdate, 0, deltaFixedTime);
    }

    if(timeSinceLastScreenScanUpdate >= ScreenScanInterval) {
        auto deltaFixedTime = timeSinceLastScreenScanUpdate;
        timeSinceLastScreenScanUpdate = 0.0;

        FireNativeEvent(AppScreenScanUpdate, 0, deltaFixedTime);
    }
}
