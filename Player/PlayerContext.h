//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_PLAYERCONTEXT_H
#define PLAZA_PLAYERCONTEXT_H

#include <Core/Module.h>
#include <Core/Entity.h>


    DeclareService(PlayerContext)

    Entity GetRuntimeFolder();
    int PlayerMain(int argc, char** argv, Module *modules, const StringRef* virtualPathMappings, const StringRef* assets);


#endif //PLAZA_PLAYERCONTEXT_H
