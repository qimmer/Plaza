//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include <Core/Strings.h>
#include "FoundationModule.h"

struct AppLoop {
    u64 AppLoopFrame;
    bool AppLoopDisabled, AppLoopKeepAlive;
};

BeginUnit(AppLoop)
    BeginComponent(AppLoop)
        RegisterProperty(u64, AppLoopFrame)
        RegisterProperty(bool, AppLoopDisabled)
        RegisterProperty(bool, AppLoopKeepAlive)
    EndComponent()
EndUnit()

API_EXPORT void RunAppLoops() {
    while(true) {
        bool any = false;
        for_entity(appLoop, appLoopData, AppLoop, {
            if(!appLoopData->AppLoopDisabled) {
                SetAppLoopFrame(appLoop, appLoopData->AppLoopFrame + 1);

                if(appLoopData->AppLoopKeepAlive) {
                    any = true;
                }
            }
        });

        CleanupStrings();

        if(!any) {
            break;
        }
    }
}
