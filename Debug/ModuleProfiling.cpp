//
// Created by Kim on 05-02-2019.
//

#include "ModuleProfiling.h"
#include <Foundation/StopWatch.h>

LocalFunction(StartMeasuringModuleLoadDuration, void, Entity module) {
    AddComponent(module, ComponentOf_ProfiledModule());

    auto stopWatch = GetProfiledModuleStartupStopWatch(module);
    SetStopWatchElapsedSeconds(stopWatch, 0.0);
    SetStopWatchRunning(stopWatch, true);
}

LocalFunction(StopMeasuringModuleLoadDuration, void, Entity module) {
    auto stopWatch = GetProfiledModuleStartupStopWatch(module);

    if(IsEntityValid(stopWatch)) {
        SetStopWatchRunning(stopWatch, false);

        Info(module, "Loaded Module (%.2f\t ms): %s", GetStopWatchElapsedSeconds(stopWatch) * 1000.0, GetUuid(module));
    }
}

BeginUnit(ModuleProfiling)
    BeginComponent(ProfiledModule)
        RegisterChildProperty(StopWatch, ProfiledModuleStartupStopWatch)
    EndComponent()

    RegisterSubscription(EventOf_ModuleLoadStarted(), StartMeasuringModuleLoadDuration, 0)
    RegisterSubscription(EventOf_ModuleLoadFinished(), StopMeasuringModuleLoadDuration, 0)

    Info(module, "\n--- Module diagnostics are now enabled ---\n");
EndUnit()