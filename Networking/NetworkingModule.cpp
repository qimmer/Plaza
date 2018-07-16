//
// Created by Kim on 15/07/2018.
//

#include "NetworkingModule.h"
#include <Networking/HttpRequest.h>
#include <Networking/HttpServer.h>
#include <Networking/Server.h>

BeginModule(Networking)
    RegisterUnit(HttpRequest)
    RegisterUnit(HttpServer)
    RegisterUnit(Server)
EndModule()