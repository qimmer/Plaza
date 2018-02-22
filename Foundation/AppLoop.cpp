//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Foundation/AppLoop.h>

#include <ctime>
#include <ratio>
#include <chrono>

using namespace std::chrono;

typedef std::chrono::high_resolution_clock clocktype;
typedef std::chrono::duration<double, std::milli> durationType;

static clocktype::time_point lastAppUpdate = clocktype::now();
static double deltaTime = 0.0;

DefineEvent(AppUpdate, AppUpdateHandler)

void AppUpdate() {
    clocktype::time_point now = clocktype::now();
    durationType delta = now - lastAppUpdate;

    deltaTime = delta.count() / 1000.0;

    lastAppUpdate = now;

    FireEvent(AppUpdate, deltaTime);
}

double GetDeltaTime() {
    return deltaTime;
}

