//
// Created by Kim on 22-07-2018.
//

#include <Networking/HttpServer.h>
#include <Json/JsonPersistance.h>
#include "RestServer.h"
#include "RestRouting.h"

BeginUnit(RestServer)
    BeginComponent(RestServer)
        RegisterBase(HttpServer)
        RegisterArrayProperty(RestRouting, RestServerRoutes)
    EndComponent()
EndUnit()