//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Foundation/AppLoop.h>
#include <Core/Debug.h>

struct AppLoop {
    u64 AppLoopFrame;
};

BeginUnit(AppLoop)
    BeginComponent(AppLoop)
        RegisterProperty(u64, AppLoopFrame)
    EndComponent()
EndUnit()

API_EXPORT void RunAppLoop(Entity appLoop) {
    auto data = GetAppLoopData(appLoop);

    if(data) {
        while(IsEntityValid(appLoop)) {
            SetAppLoopFrame(appLoop, data->AppLoopFrame++);
        }
    }
}