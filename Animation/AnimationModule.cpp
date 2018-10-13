//
// Created by Kim on 11-10-2018.
//

#include "AnimationModule.h"
#include "Animation.h"
#include "AnimationPlayer.h"
#include <Foundation/StopWatch.h>

struct AnimationModule {
    Entity AnimationStopWatch;
};

BeginModule(Animation)
    RegisterUnit(AnimationModule)
    RegisterUnit(Animation)
    RegisterUnit(AnimationPlayer)

    AddComponent(module, ComponentOf_AnimationModule());
    auto animationStopWatch = GetAnimationStopWatch(module);
    SetStopWatchRunning(animationStopWatch, true);

EndModule()

BeginUnit(AnimationModule)
    BeginComponent(AnimationModule)
        RegisterChildProperty(StopWatch, AnimationStopWatch)
    EndComponent()
EndUnit()