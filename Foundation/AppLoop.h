//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_APPLOOP_H
#define PLAZA_APPLOOP_H

#include <Core/NativeUtils.h>

struct AppLoop {
    u64 AppLoopFrame;
    float AppLoopOrder;
};

Unit(AppLoop)
    Component(AppLoop)
        Property(u64, AppLoopFrame)
        Property(float, AppLoopOrder)

    Function(RunAppLoops, void)
    Function(Quit, void)
    Function(UpdateAppLoops, bool)

#define AppLoopOrder_Rendering 10.0f
#define AppLoopOrder_Update 1.0f
#define AppLoopOrder_Input 0.1f

#endif //PLAZA_APPLOOP_H
