//
// Created by Kim Johannsen on 19-03-2018.
//

#include "LogicModule.h"
#include "State.h"
#include "ToggleState.h"
#include "InvertState.h"
#include "PropertyBinding.h"

DefineModule(Logic)
    ModuleType(State)
    ModuleType(ToggleState)
    ModuleType(InvertState)
    ModuleType(PropertyBinding)

    ModuleService(ToggleState)
    ModuleService(InvertState)
    ModuleService(PropertyBinding)
EndModule()