//
// Created by Kim Johannsen on 24/01/2018.
//

#include "CoreModule.h"
#include "Entity.h"

DefineModule(Core)
    ModuleService(Entity)
    ModuleService(Hierarchy)
    ModuleService(FunctionCallService)

    ModuleType(Hierarchy)
EndModule()
