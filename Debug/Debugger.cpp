//
// Created by Kim on 28-07-2018.
//

#include <Core/Debug.h>
#include <Foundation/AppLoop.h>
#include <Core/Identification.h>
#include <Networking/Replication.h>
#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>
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
        auto path = argv[i];

        if(strstr(path, ".dll") || strstr(path, ".so") || strstr(path, ".dylib")) {
            auto module = LoadPlazaModule(path);
            if(IsEntityValid(module)) {
                printf("Module '%s' Loaded from '%s'.\n", GetName(module), argv[i]);
            }
        } else {
            char vpath[512];
            snprintf(vpath, sizeof(vpath), "file://%s", path);

            auto module = AddModules(GetModuleRoot());
            SetStreamPath(module, vpath);
            SetPersistancePointLoading(module, true);

            printf("Module '%s' Loaded from '%s'.\n", GetName(module), path);
        }

    }

    RunAppLoops();

    hasRunAsExpected = true;

    return hasRunAsExpected ? 0 : 1;
}