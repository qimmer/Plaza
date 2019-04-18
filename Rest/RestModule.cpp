//
// Created by Kim on 15/07/2018.
//

#include <Foundation/FoundationModule.h>
#include <Json/JsonModule.h>
#include <Networking/NetworkingModule.h>
#include "RestModule.h"
#include "RestEntityRouting.h"
#include "RestResourceRouting.h"

BeginModule(Rest)
    RegisterDependency(Core)
    RegisterDependency(Foundation)
    RegisterDependency(Json)
    RegisterDependency(Networking)

    RegisterUnit(RestEntityRouting)
    RegisterUnit(RestResourceRouting)
EndModule()