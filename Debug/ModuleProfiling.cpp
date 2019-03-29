//
// Created by Kim on 05-02-2019.
//

#include "ModuleProfiling.h"
#include <Foundation/StopWatch.h>
#include <Foundation/PersistancePoint.h>
#include <Foundation/Stream.h>

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

        Info(module, "Loaded Module (%.2f\t ms): %s", GetStopWatchElapsedSeconds(stopWatch) * 1000.0, GetIdentification(module).Uuid);
    }
}

LocalFunction(OnPersistancePointLoadingChanged, void, Entity entity, bool oldValue, bool newValue) {
    AddComponent(entity, ComponentOf_ProfiledModule());
    auto stopWatch = GetProfiledModuleStartupStopWatch(entity);

    if(!oldValue && newValue && stopWatch) {
        SetStopWatchElapsedSeconds(stopWatch, 0.0);
        SetStopWatchRunning(stopWatch, true);
    }

    if(oldValue && !newValue && stopWatch) {
        SetStopWatchRunning(stopWatch, false);

        Info(entity, "Loaded Data (%.2f\t ms): %s", GetStopWatchElapsedSeconds(stopWatch) * 1000.0, GetStreamPath(entity));
    }
}

BeginUnit(ModuleProfiling)
    BeginComponent(ProfiledModule)
        RegisterChildProperty(StopWatch, ProfiledModuleStartupStopWatch)
    EndComponent()

    RegisterSubscription(EventOf_ModuleLoadStarted(), StartMeasuringModuleLoadDuration, 0)
    RegisterSubscription(EventOf_ModuleLoadFinished(), StopMeasuringModuleLoadDuration, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PersistancePointLoading()), OnPersistancePointLoadingChanged, 0)

    Info(module, "\n--- Module diagnostics are now enabled ---\n");
EndUnit()