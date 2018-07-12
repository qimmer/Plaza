//
// Created by Kim Johannsen on 04-03-2018.
//

#include <Foundation/FoundationModule.h>
#include "JsonModule.h"
#include "JsonPersistance.h"

BeginModule(Json)
    RegisterDependency(Foundation)
    RegisterUnit(JsonPersistance)
EndModule()
