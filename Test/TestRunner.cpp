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
        auto module = LoadPlazaModule(argv[i]);
        if(IsEntityValid(module)) {
            printf("Module '%s' Loaded from '%s'.\n", GetIdentification(module).Uuid, argv[i]);
        }
    }

    printf("%s", "\n");

    s32 numFailed = 0, numSucceeded = 0;
    for_entity(entity, ComponentOf_Test()) {
        auto argument = MakeVariant(Entity, entity);
        auto result = CallFunction(entity, 1, &argument);
        printf("%s %s.\n", GetIdentification(entity).Uuid, (result.as_u32 == 0) ? "Succeeded" : "Failed");

        if(result.as_u32 == 0) {
            numSucceeded++;
        } else {
            numFailed++;
        }
    }

    printf("\nTest Results:\n=================\n%d Tests Passed\n%d Tests Failed\n=================", numSucceeded, numFailed);

    hasRunAsExpected = true;
}