//
// Created by Kim on 06/07/2018.
//

#ifndef PLAZA_STOPWATCH_H
#define PLAZA_STOPWATCH_H

#include <Core/NativeUtils.h>
#include <Foundation/AppLoop.h>

Unit(StopWatch)
    Component(StopWatch)
        Property(bool, StopWatchRunning)
        Property(double, StopWatchElapsedSeconds)

    Declare(AppLoop, StopWatchUpdate)

#endif //PLAZA_STOPWATCH_H
