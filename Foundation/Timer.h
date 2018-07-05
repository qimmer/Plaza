//
// Created by Kim on 05/07/2018.
//

#ifndef PLAZA_TIMER_H
#define PLAZA_TIMER_H

#include <Core/Component.h>

Function(GetTimeSinceStart, double)

Unit(Timer)

Component(Timer)
    Property(double, TimerInterval)
    Property(bool, TimerStarted)
    Property(bool, TimerRepeat)



#endif //PLAZA_TIMER_H
