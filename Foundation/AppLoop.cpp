//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/NativeUtils.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include <Core/Strings.h>
#include "FoundationModule.h"

#include <algorithm>
#include <Core/System.h>

static bool quit = false;

BeginUnit(AppLoop)
    RegisterFunction(RunAppLoop)

    BeginComponent(AppLoop)
        RegisterProperty(u64, AppLoopFrame)
        RegisterProperty(double, AppLoopDeltaTime)
        RegisterProperty(bool, AppLoopQuit)
    EndComponent()
EndUnit()

API_EXPORT void RunAppLoop() {
    auto appLoop = CreateEntity();
    SetIdentification(appLoop, {"AppLoop"});
    SetOwnership(appLoop, {GetRoot(), 0});

    auto appLoopData = GetAppLoop(appLoop);
    while(!appLoopData.AppLoopQuit) {
        appLoopData.AppLoopFrame++;

        SetAppLoop(appLoop, appLoopData);
        ProcessSystems();

        appLoopData = GetAppLoop(appLoop);
    }

    DestroyEntity(appLoop);
}

API_EXPORT void Quit() {
    quit = true;
}
