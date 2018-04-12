//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_APPLOOP_H
#define PLAZA_APPLOOP_H

#include <Core/Delegate.h>

typedef void(*AppUpdateHandler)(double deltaTime);

void AppUpdate();
double GetTimeSinceStart();

void SetScreenScanInterval(double interval);
double GetScreenScanInterval();

DeclareEvent(AppUpdate, AppUpdateHandler)
DeclareEvent(AppFixedUpdate, AppUpdateHandler)
DeclareEvent(AppScreenScanUpdate, AppUpdateHandler)

#endif //PLAZA_APPLOOP_H
