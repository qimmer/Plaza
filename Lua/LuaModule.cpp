//
// Created by Kim Johannsen on 05-04-2018.
//

#include "LuaModule.h"
#include "LuaScript.h"
#include "LuaBinding.h"

BeginModule(Lua)
    ModuleType(LuaScript)

    ModuleService(LuaScript)
    ModuleService(LuaBinding)
EndModule()