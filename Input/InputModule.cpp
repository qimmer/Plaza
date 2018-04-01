//
// Created by Kim Johannsen on 19-03-2018.
//

#include <Logic/LogicModule.h>
#include "InputModule.h"
#include "InputState.h"

DefineModule(Input)
    ModuleDependency(Logic)

    ModuleType(InputState)
EndModule()