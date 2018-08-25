//
// Created by Kim on 28-07-2018.
//

#include <Core/Debug.h>
#include <fstream>
#include <Foundation/AppLoop.h>
#include <Core/Identification.h>
#include <Networking/Replication.h>
#include "DebugModule.h"

static bool hasRunAsExpected = false;

void OnExit() {
    if(!hasRunAsExpected) {
        DebuggerBreak();
    }
}

int main(int argc, char** argv) { \
    atexit(OnExit);

    ModuleOf_Debug();

    for(auto i = 1; i < argc; ++i) {
        auto module = LoadPlazaModule(argv[i]);
        if(IsEntityValid(module)) {
            printf("Module '%s' Loaded from '%s'.\n", GetName(module), argv[i]);
        }
    }

    AddComponent(GetModuleRoot(), ComponentOf_Replication());

    RunAppLoops();

    hasRunAsExpected = true;

    return hasRunAsExpected ? 0 : 1;
}