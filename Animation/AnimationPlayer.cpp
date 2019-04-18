//
// Created by Kim on 11-10-2018.
//

#include "AnimationPlayer.h"
#include "Animation.h"
#include "AnimationModule.h"
#include <Foundation/StopWatch.h>
#include <Foundation/AppNode.h>
#include <Core/Instance.h>

static inline void EvaluateLayer(Entity animationPlayer, Entity layer, const AnimationPlayerLayer& layerData, double deltaTime) {
    auto newTime = layerData.AnimationPlayerLayerTime;
    newTime += deltaTime * layerData.AnimationPlayerLayerSpeed;

    auto newLayerData = layerData;
    newLayerData.AnimationPlayerLayerTime = newTime;
    SetAnimationPlayerLayer(layer, newLayerData);

    auto animationData = GetAnimation(layerData.AnimationPlayerLayerAnimation);
    for(auto track : animationData.AnimationTracks) {
        auto value = EvaluateAnimationFrame(track, newTime, layerData.AnimationPlayerLayerLooping);
        if(!value.type) continue;

        auto property = GetAnimationTrack(track).AnimationTrackProperty;
        value = Cast(value, GetProperty(property).PropertyType);
        SetPropertyValue(property, animationPlayer, value);
    }
}

static void OnUpdateAnimation(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    AnimationPlayer playerData;

    for_entity_data(animationPlayer, ComponentOf_AnimationPlayer(), &playerData) {
        if(!IsEntityValid(GetAppNode(animationPlayer).AppNodeRoot)) continue;

        for(auto layer : playerData.AnimationPlayerLayers) {
            auto layerData = GetAnimationPlayerLayer(layer);
            if(layerData.AnimationPlayerLayerSpeed != 0.0f) {
                EvaluateLayer(animationPlayer, layer, layerData, newData.AppLoopDeltaTime);
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
        RegisterBase(AppNode)
        RegisterArrayProperty(AnimationPlayerLayer, AnimationPlayerLayers)
    EndComponent()

    RegisterDeferredSystem(OnUpdateAnimation, ComponentOf_AppLoop(), AppLoopOrder_Update)
EndUnit()
