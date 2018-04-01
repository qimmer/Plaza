//
// Created by Kim Johannsen on 19-03-2018.
//

#include "LogicModule.h"
#include "State.h"
#include "ToggleState.h"
#include "InvertState.h"
#include "PropertyState.h"

DefineModule(Logic)
    ModuleType(State)
    ModuleType(ToggleState)
    ModuleType(InvertState)
    ModuleType(PropertyState)

    ModuleService(ToggleState)
    ModuleService(InvertState)
    ModuleService(PropertyState)
EndModule()