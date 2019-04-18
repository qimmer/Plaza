//
// Created by Kim on 28-07-2018.
//

#include <Core/Debug.h>
#include <Core/CoreModule.h>
#include <Foundation/AppLoop.h>
#include <Foundation/FoundationModule.h>
#include <Core/Identification.h>
#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>
#include <Json/JsonModule.h>
#include <File/FileModule.h>
#include <Networking/NetworkingModule.h>
#include <Rest/RestModule.h>
#include "DebugModule.h"

static bool hasRunAsExpected = false;

void OnExit() {
    if(!hasRunAsExpected) {
        DebuggerBreak();
    }
}

int main(int argc, char** argv) {
    atexit(OnExit);

    for(auto i = 1; i < argc; ++i) {
        auto path = argv[i];

        if(strstr(path, "-verbose=")) {
            //VerboseTag = path + strlen("-verbose=");
        }
    }

    RegisterDependency(Core)
    RegisterDependency(Foundation)
    RegisterDependency(File)
    RegisterDependency(Json)
    RegisterDependency(Debug) // Enables debugging server

    for(auto i = 1; i < argc; ++i) {
        auto path = argv[i];

        if(strstr(path, ".dll") || strstr(path, ".so") || strstr(path, ".dylib")) {
            LoadPlazaModule(path);
        }
    }

    RunAppLoop();

    DestroyEntity(GetRoot());

    //DumpTree(GetModuleRoot());

    hasRunAsExpected = true;

    return hasRunAsExpected ? 0 : 1;
}