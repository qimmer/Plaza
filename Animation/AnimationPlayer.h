//
// Created by Kim on 11-10-2018.
//

#ifndef PLAZA_ANIMATIONPLAYER_H
#define PLAZA_ANIMATIONPLAYER_H

#include <Core/NativeUtils.h>

Unit(AnimationPlayer)
    Component(AnimationPlayerLayer)
        ReferenceProperty(Animation, AnimationPlayerLayerAnimation)
        Property(float, AnimationPlayerLayerWeight)
        Property(float, AnimationPlayerLayerSpeed)
        Property(float, AnimationPlayerLayerTime)
        Property(bool, AnimationPlayerLayerLooping)

    Component(AnimationPlayer)
        ArrayProperty(AnimationPlayerLayer, AnimationPlayerLayers)

    Declare(AppLoop, Animation)
    Declare(StopWatch, Animation)

#endif //PLAZA_ANIMATIONPLAYER_H
