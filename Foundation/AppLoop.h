//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_APPLOOP_H
#define PLAZA_APPLOOP_H

#include <Core/Event.h>

void AppUpdate();
double GetTimeSinceStart();

void SetScreenScanInterval(double interval);
double GetScreenScanInterval();

DeclareEvent(AppUpdate, Entity context, double deltaTime)
DeclareEvent(AppFixedUpdate, Entity context, double deltaTime)
DeclareEvent(AppScreenScanUpdate, Entity context, double deltaTime)

#endif //PLAZA_APPLOOP_H
