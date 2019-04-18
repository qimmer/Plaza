//
// Created by Kim on 05-02-2019.
//

#include "ModuleProfiling.h"
#include <Foundation/StopWatch.h>
#include <Foundation/PersistancePoint.h>
#include <Foundation/Stream.h>

BeginUnit(ModuleProfiling)
    BeginComponent(ProfiledModule)
        BeginChildProperty(ProfiledModuleStartupStopWatch)
    EndChildProperty()
    EndComponent()

    Info(module, "\n--- Module diagnostics are now enabled ---\n");
EndUnit()