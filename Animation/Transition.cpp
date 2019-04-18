//
// Created by Kim on 08-01-2019.
//

#include "Transition.h"
#include "Animation.h"
#include <Core/Algorithms.h>
#include <Foundation/StopWatch.h>
#include <Foundation/AppNode.h>

static void OnUpdateTransitions(Entity entity, const AppLoop& oldData, const AppLoop& newValue) {
    static eastl::fixed_vector<Entity, 128> finishedTransitions;

    finishedTransitions.clear();
    ActiveTransition transitionData;
    for_entity_data(activeTransition, ComponentOf_ActiveTransition(), &transitionData) {
        if(!IsEntityValid(GetAppNode(activeTransition).AppNodeRoot)) continue;

        auto t = Clamp(transitionData.ActiveTransitionTime / transitionData.ActiveTransitionDuration, 0.0f, 1.0f);

        auto value = Interpolate(transitionData.ActiveTransitionSourceValue, transitionData.ActiveTransitionDestinationValue, t);

        SetPropertyValue(transitionData.ActiveTransitionProperty, GetOwnership(activeTransition).Owner, value);

        if(t >= 1.0f) {
            finishedTransitions.push_back(activeTransition);
        }
    }

    for(auto& finished : finishedTransitions) {
        auto context = GetOwnership(finished).Owner;
        auto data = GetTransitioningEntity(context);
        data.ActiveTransitions.Remove(data.ActiveTransitions.GetIndex(finished));
        SetTransitioningEntity(context, data);

        if(!data.ActiveTransitions.GetSize()) {
            RemoveComponent(context, ComponentOf_TransitioningEntity());
        }
    }
}

API_EXPORT void Transition(Entity entity, Entity property, Variant destinationValue, float time) {
    if(time <= 0.0f) {
        SetPropertyValue(property, entity, destinationValue);
        return;
    }

    ActiveTransition data;
    Entity activeTransition = 0;
    auto entityData = GetTransitioningEntity(entity);
    for(auto transition : entityData.ActiveTransitions) {
        data = GetActiveTransition(transition);
        if(data.ActiveTransitionProperty == property) {
            activeTransition = transition;
            break;
        }
    }

    if(!activeTransition) {
        activeTransition = CreateEntity();
        data.ActiveTransitionProperty = property;
        entityData.ActiveTransitions.Add(activeTransition);
        SetTransitioningEntity(entity, entityData);
    }

    data.ActiveTransitionSourceValue = GetPropertyValue(property, entity);
    data.ActiveTransitionDestinationValue = destinationValue;
    data.ActiveTransitionTime = 0.0f;
    data.ActiveTransitionDuration = time;
    SetActiveTransition(activeTransition, data);
}

BeginUnit(Transition)
    BeginComponent(TransitioningEntity)
        RegisterArrayProperty(ActiveTransition, ActiveTransitions)
    EndComponent()
    BeginComponent(ActiveTransition)
        RegisterProperty(float, ActiveTransitionDuration)
        RegisterProperty(float, ActiveTransitionTime)
        RegisterProperty(Variant, ActiveTransitionSourceValue)
        RegisterProperty(Variant, ActiveTransitionDestinationValue)
        RegisterReferenceProperty(Property, ActiveTransitionProperty)
    EndComponent()

    RegisterDeferredSystem(OnUpdateTransitions, ComponentOf_AppLoop(), AppLoopOrder_Update)
EndUnit()