//
// Created by Kim on 15/07/2018.
//

#include <Foundation/AppLoop.h>

#include "Server.h"

struct Server {
    u16 ServerPort;
};

BeginUnit(Server)
    BeginComponent(Server)
        RegisterProperty(u16, ServerPort)
     EndComponent()
EndUnit()