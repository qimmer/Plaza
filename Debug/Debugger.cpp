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

    SetStreamPath(GetModuleRoot(), "file://Modules.json");
    SetPersistancePointLoading(GetModuleRoot(), true);

    RunAppLoops();

    hasRunAsExpected = true;

    return hasRunAsExpected ? 0 : 1;
}