//
// Created by Kim on 15/07/2018.
//

#include "RestRouting.h"

struct RestRouting {
    char RestRoutingRoute[128];
};

BeginUnit(RestRouting)
    BeginComponent(RestRouting)
        RegisterProperty(StringRef, RestRoutingRoute)
    EndComponent()

    RegisterEvent(RestRequest)
EndUnit()