//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/NativeUtils.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include <Core/Strings.h>
#include "FoundationModule.h"

#include <algorithm>

static Vector<Entity> sortedAppLoops;
static bool quit = false;

static bool CompareAppLoopOrder(const Entity &a, const Entity &b) {
    return GetAppLoopOrder(a) < GetAppLoopOrder(b);
}

#define Verbose_AppLoop "apploop"

LocalFunction(OnSortAppLoops, void, Entity changedAppLoop) {
    sortedAppLoops.clear();

    for_entity(appLoop, appLoopData, AppLoop) {
        sortedAppLoops.push_back(appLoop);
    }

    std::sort(sortedAppLoops.begin(), sortedAppLoops.end(), CompareAppLoopOrder);

    Verbose(Verbose_AppLoop, "%s", "Sorting AppLoops ...");
    for(auto& appLoop : sortedAppLoops) {
        Verbose(Verbose_AppLoop, "    %s", GetUuid(appLoop));
    }
    Verbose(Verbose_AppLoop, "%s", "");
}

BeginUnit(AppLoop)
    BeginComponent(AppLoop)
        RegisterProperty(u64, AppLoopFrame)
        RegisterProperty(float, AppLoopOrder)
    EndComponent()

    RegisterFunction(RunAppLoops)
    RegisterFunction(Quit)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopOrder()), OnSortAppLoops, 0)
EndUnit()

API_EXPORT bool UpdateAppLoops() {
    bool any = false;

    for(auto i = 0; i < sortedAppLoops.size(); ++i) {
        if(IsEntityValid(sortedAppLoops[i])) {
            any = true;
            SetAppLoopFrame(sortedAppLoops[i], GetAppLoopFrame(sortedAppLoops[i]) + 1);
        }
    }

    return any;
}

API_EXPORT void RunAppLoops() {
    while(!quit) {

        auto any = UpdateAppLoops();

        CleanupStrings();

        if(!any) {
            break;
        }
    }
}

API_EXPORT void Quit() {
    quit = true;
}
