//
// Created by Kim on 05/07/2018.
//

#include <Foundation/Timer.h>
#include <Foundation/StopWatch.h>
#include <Core/Debug.h>

struct Timer {
    double TimerInterval;
    bool TimerRepeat;
    double LastStopWatchTime;
};

LocalFunction(OnStopWatchElapsedSecondsChanged, void, Entity stopWatch, double oldTime, double newTime) {
    auto timer = GetTimerData(stopWatch);

    if(timer) {
        if(newTime >= (timer->LastStopWatchTime + timer->TimerInterval)) {
            timer->LastStopWatchTime = newTime;

            Variant argument = MakeVariant(Entity, stopWatch);

            FireEventFast(EventOf_TimerTick(), 1, &argument);

            if(!timer->TimerRepeat) {
                SetStopWatchRunning(stopWatch, false);
                SetStopWatchElapsedSeconds(stopWatch, 0.0);
            }

        }
    }
}

LocalFunction(OnStopWatchRunningChanged, void, Entity timer, bool oldValue, bool newValue) {
    if(newValue) {
        auto data = GetTimerData(timer);
        if(data) {
            data->LastStopWatchTime = GetStopWatchElapsedSeconds(timer);
        }
    }
}

BeginUnit(Timer)
    BeginComponent(Timer)
        RegisterBase(StopWatch)
        RegisterProperty(double, TimerInterval)
        RegisterProperty(bool, TimerRepeat)
    EndComponent()
    RegisterEvent(TimerTick)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_StopWatchElapsedSeconds()), OnStopWatchElapsedSecondsChanged, 0)
EndUnit()

