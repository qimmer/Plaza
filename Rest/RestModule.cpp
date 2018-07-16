//
// Created by Kim on 15/07/2018.
//

#include <Foundation/FoundationModule.h>
#include <Json/JsonModule.h>
#include "RestModule.h"

BeginModule(Rest)
    RegisterDependency(Core)
    RegisterDependency(Foundation)
    RegisterDependency(Json)
EndModule()