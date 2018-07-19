//
// Created by Kim on 05/07/2018.
//

#include <Core/Module.h>
#include <cstdio>
#include <cstdlib>
#include <Core/Debug.h>
#include <Core/Identification.h>
#include "TestRunner.h"
#include "Test.h"

static bool hasRunAsExpected = false;

void OnExit() {
    if(!hasRunAsExpected) {
        DebuggerBreak();
    }
}

int main(int argc, char** argv) { \
    atexit(OnExit);

    for(auto i = 1; i < argc; ++i) {
        auto module = LoadModule(argv[i]);
        if(IsEntityValid(module)) {
            printf("Module '%s' Loaded from '%s'.\n", GetName(module), argv[i]);
        }
    }

    printf("%s", "\n");

    s32 numFailed = 0, numSucceeded = 0;
    for(auto i = 0; i < GetComponentMax(ComponentOf_Test()); ++i) {
        auto entity = GetComponentEntity(ComponentOf_Test(), i);
        if(IsEntityValid(entity)) {
            u32 result = -1;

            const Type argumentTypes[] = {TypeOf_Entity};
            const void* argumentPtrs[] = {&entity};

            CallFunction(entity, &result, 1, argumentTypes, argumentPtrs);
            printf("%s %s.\n", GetName(entity), (result == 0) ? "Succeeded" : "Failed");

            if(result == 0) {
                numSucceeded++;
            } else {
                numFailed++;
            }
        }

    }

    printf("\nTest Results:\n=================\n%d Tests Passed\n%d Tests Failed\n=================", numSucceeded, numFailed);

    hasRunAsExpected = true;
}