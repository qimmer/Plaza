//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_PLAYERCONTEXT_H
#define PLAZA_PLAYERCONTEXT_H

#include <Core/Service.h>
#include <Core/Module.h>
#include <Core/Entity.h>


    DeclareService(PlayerContext)

    Entity GetPlayerContext();
    Entity GetRuntimeFolder();
    int PlayerMain(int argc, char** argv, Module *modules, StringRef* assets, StringRef title);


#endif //PLAZA_PLAYERCONTEXT_H
