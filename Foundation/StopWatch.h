//
// Created by Kim on 06/07/2018.
//

#ifndef PLAZA_STOPWATCH_H
#define PLAZA_STOPWATCH_H

#include <Core/NativeUtils.h>
#include <Foundation/AppLoop.h>

struct StopWatch {
    bool StopWatchRunning;
    double StopWatchElapsedSeconds;
    Entity StopWatchUpdateLoop;
    u64 lastTime;
};

Unit(StopWatch)
    Component(StopWatch)
        Property(bool, StopWatchRunning)
        Property(double, StopWatchElapsedSeconds)

#endif //PLAZA_STOPWATCH_H
