//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Foundation/FoundationModule.h>
#include "ModuleDefinition.h"
#include "ServiceDefinition.h"

DefineModule(DataModel)
        ModuleDependency(Foundation)

        ModuleType(ModuleDefinition)
        ModuleType(ServiceDefinition)
EndModule()
