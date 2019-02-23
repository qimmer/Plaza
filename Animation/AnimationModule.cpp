//
// Created by Kim on 11-10-2018.
//

#include "AnimationModule.h"
#include "Animation.h"
#include "AnimationPlayer.h"
#include "Transition.h"
#include <Foundation/StopWatch.h>

BeginModule(Animation)
    RegisterUnit(Animation)
    RegisterUnit(AnimationPlayer)
    RegisterUnit(Transition)
EndModule()
