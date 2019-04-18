//
// Created by Kim on 11-10-2018.
//

#ifndef PLAZA_ANIMATIONPLAYER_H
#define PLAZA_ANIMATIONPLAYER_H

#include <Core/NativeUtils.h>

struct AnimationPlayerLayer {
    Entity AnimationPlayerLayerAnimation;
    float AnimationPlayerLayerWeight, AnimationPlayerLayerSpeed, AnimationPlayerLayerTime;
    bool AnimationPlayerLayerLooping;
};

struct AnimationPlayer {
    ChildArray AnimationPlayerLayers;
};

Unit(AnimationPlayer)
    Component(AnimationPlayerLayer)
        ReferenceProperty(Animation, AnimationPlayerLayerAnimation)
        Property(float, AnimationPlayerLayerWeight)
        Property(float, AnimationPlayerLayerSpeed)
        Property(float, AnimationPlayerLayerTime)
        Property(bool, AnimationPlayerLayerLooping)

    Component(AnimationPlayer)
        ArrayProperty(AnimationPlayerLayer, AnimationPlayerLayers)

#endif //PLAZA_ANIMATIONPLAYER_H
