//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include "FoundationModule.h"

struct AppLoop {
    u64 AppLoopFrame;
};

BeginUnit(AppLoop)
    BeginComponent(AppLoop)
        RegisterProperty(u64, AppLoopFrame)
    EndComponent()
EndUnit()

API_EXPORT void RunAppLoop(Entity appLoop) {
    AddComponent(appLoop, ComponentOf_AppLoop());
    auto data = GetAppLoopData(appLoop);

    if(data) {
        while(IsEntityValid(appLoop)) {
            SetAppLoopFrame(appLoop, ++data->AppLoopFrame);
        }
    }
}

API_EXPORT void RunMainAppLoop() {
    auto appLoop = ModuleOf_Foundation();

    RunAppLoop(appLoop);
}