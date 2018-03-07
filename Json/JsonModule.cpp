//
// Created by Kim Johannsen on 04-03-2018.
//

#include <Foundation/FoundationModule.h>
#include "JsonModule.h"
#include "JsonPersistance.h"

DefineModule(Json)
        ModuleDependency(Foundation)

        ModuleService(JsonPersistance)
EndModule()
