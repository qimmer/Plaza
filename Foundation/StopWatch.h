//
// Created by Kim on 06/07/2018.
//

#ifndef PLAZA_STOPWATCH_H
#define PLAZA_STOPWATCH_H

#include <Core/NativeUtils.h>

Unit(StopWatch)
    Component(StopWatch)
        ChildProperty(AppLoop, StopWatchUpdateLoop)
        Property(bool, StopWatchRunning)
        Property(double, StopWatchElapsedSeconds)

#endif //PLAZA_STOPWATCH_H
