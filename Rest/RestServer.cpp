//
// Created by Kim on 22-07-2018.
//

#include <Networking/HttpServer.h>
#include "RestServer.h"
#include "RestRouting.h"

struct RestServer {
    Vector(RestServerRoutes, Entity, 128)
};

BeginUnit(RestServer)
    BeginComponent(RestServer)
        RegisterBase(HttpServer)
        RegisterArrayProperty(RestRouting, RestServerRoutes)
    EndComponent()
EndUnit()