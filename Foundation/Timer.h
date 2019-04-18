//
// Created by Kim on 05/07/2018.
//

#ifndef PLAZA_TIMER_H
#define PLAZA_TIMER_H

#include <Core/Component.h>

struct Timer {
    u64 TimerTicks;
    double TimerInterval;
    double LastStopWatchTime;
    bool TimerRepeat;
};

Unit(Timer)
    Component(Timer)
        Property(double, TimerInterval)
        Property(bool, TimerRepeat)
        Property(u64, TimerTicks)

#endif //PLAZA_TIMER_H
