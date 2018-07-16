//
// Created by Kim on 15/07/2018.
//

#include "RestEntityRouting.h"

struct RestEntityRouting {
    Entity RestEntityRoutingRoot;
};

BeginUnit(RestEntityRouting)
    BeginComponent(RestEntityRouting)
        RegisterProperty(Entity, RestEntityRoutingRoot)
    EndComponent()
EndUnit()