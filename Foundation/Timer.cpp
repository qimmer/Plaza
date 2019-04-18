//
// Created by Kim on 05/07/2018.
//

#include <Core/NativeUtils.h>
#include <Foundation/Timer.h>
#include <Foundation/StopWatch.h>

static void OnStopWatchChanged(Entity stopWatch, const StopWatch& oldValue, const StopWatch& newValue) {
    auto timerData = GetTimer(stopWatch);

    if(timerData.TimerInterval != 0.0) {
        if(newValue.StopWatchRunning != oldValue.StopWatchRunning) {
            timerData.LastStopWatchTime = newValue.StopWatchElapsedSeconds;
            SetTimer(stopWatch, timerData);
        }

        if(newValue.StopWatchElapsedSeconds != oldValue.StopWatchElapsedSeconds && newValue.StopWatchElapsedSeconds >= (timerData.LastStopWatchTime + timerData.TimerInterval)) {
            timerData.LastStopWatchTime = newValue.StopWatchElapsedSeconds;
            timerData.TimerTicks++;
            SetTimer(stopWatch, timerData);

            if(!timerData.TimerRepeat) {
                auto stopWatchData = newValue;
                stopWatchData.StopWatchRunning = false;
                stopWatchData.StopWatchElapsedSeconds = 0.0;
                SetStopWatch(stopWatch, stopWatchData);
            }

        }
    }
}

BeginUnit(Timer)
    BeginComponent(Timer)
        RegisterBase(StopWatch)
        RegisterProperty(double, TimerInterval)
        RegisterProperty(bool, TimerRepeat)
        RegisterProperty(u64, TimerTicks)
    EndComponent()

    RegisterSystem(OnStopWatchChanged, ComponentOf_StopWatch())
EndUnit()

