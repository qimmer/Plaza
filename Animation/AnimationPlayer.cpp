//
// Created by Kim on 11-10-2018.
//

#include "AnimationPlayer.h"
#include "Animation.h"
#include "AnimationModule.h"
#include <Foundation/StopWatch.h>

struct AnimationPlayerLayer {
    Entity AnimationPlayerLayerAnimation;
    float AnimationPlayerLayerWeight, AnimationPlayerLayerSpeed, AnimationPlayerLayerTime;
    bool AnimationPlayerLayerLooping;
};

struct AnimationPlayer {
    Vector(AnimationPlayerLayers, Entity, 16)
};

LocalFunction(OnElapsedChanged, void, Entity stopWatch, double oldElapsed, double newElapsed) {
    auto deltaTime = newElapsed - oldElapsed;

    for_entity(animationPlayer, playerData, AnimationPlayer) {
        for_children(layer, AnimationPlayerLayers, animationPlayer) {
            auto layerData = GetAnimationPlayerLayerData(layer);
            if(layerData->AnimationPlayerLayerSpeed != 0.0f) {
                auto newTime = layerData->AnimationPlayerLayerTime;
                newTime += deltaTime * layerData->AnimationPlayerLayerSpeed;
                SetAnimationPlayerLayerTime(layer, newTime);

                for_children(track, AnimationTracks, GetAnimationPlayerLayerAnimation(layer)) {
                    auto value = EvaluateAnimationFrame(track, newTime, layerData->AnimationPlayerLayerLooping);

                    auto property = GetAnimationTrackProperty(track);
                    value = Cast(value, GetPropertyType(property));
                    SetPropertyValue(property, animationPlayer, &value.data);
                }

            }
        }
    }
}

BeginUnit(AnimationPlayer)
    BeginComponent(AnimationPlayerLayer)
        RegisterReferenceProperty(Animation, AnimationPlayerLayerAnimation)
        RegisterProperty(float, AnimationPlayerLayerWeight)
        RegisterProperty(float, AnimationPlayerLayerSpeed)
        RegisterProperty(float, AnimationPlayerLayerTime)
        RegisterProperty(bool, AnimationPlayerLayerLooping)
    EndComponent()

    BeginComponent(AnimationPlayer)
        RegisterArrayProperty(AnimationPlayerLayer, AnimationPlayerLayers)
    EndComponent()

    RegisterSubscription(StopWatchElapsedSecondsChanged, OnElapsedChanged, GetAnimationStopWatch(module))
EndUnit()
