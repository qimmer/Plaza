//
// Created by Kim on 08-01-2019.
//

#include "Transition.h"
#include "Animation.h"
#include <Core/Algorithms.h>
#include <Foundation/StopWatch.h>
#include <Foundation/AppNode.h>

LocalFunction(OnUpdateTransitions, void) {
    auto deltaTime = GetStopWatchElapsedSeconds(StopWatchOf_Transition());
    SetStopWatchElapsedSeconds(StopWatchOf_Transition(), 0.0);

    static Vector<Entity> finishedTransitions;

    finishedTransitions.clear();
    for_entity(activeTransition, transitionData, ActiveTransition) {
        if(!IsEntityValid(GetAppNodeRoot(activeTransition))) continue;

        auto t = Clamp(transitionData->ActiveTransitionTime / transitionData->ActiveTransitionDuration, 0.0f, 1.0f);

        auto value = Interpolate(transitionData->ActiveTransitionSourceValue, transitionData->ActiveTransitionDestinationValue, t);

        SetPropertyValue(transitionData->ActiveTransitionProperty, GetOwner(activeTransition), value);

        if(t >= 1.0f) {
            finishedTransitions.push_back(activeTransition);
        }
    }

    for(auto& finished : finishedTransitions) {
        auto context = GetOwner(finished);
        RemoveActiveTransitionsByValue(context, finished);

        u32 numActive = 0;
        GetActiveTransitions(context, &numActive);
        if(numActive == 0) {
            RemoveComponent(context, ComponentOf_TransitioningEntity());
        }
    }
}

API_EXPORT void Transition(Entity entity, Entity property, Variant destinationValue, float time) {
    if(time <= 0.0f) {
        SetPropertyValue(property, entity, destinationValue);
        return;
    }

    ActiveTransition *data = NULL;
    Entity activeTransition = 0;

    for_children(transition, ActiveTransitions, entity) {
        data = GetActiveTransitionData(transition);
        if(data->ActiveTransitionProperty == property) {
            activeTransition = transition;
            break;
        }
    }

    if(!data) {
        activeTransition = AddActiveTransitions(entity);
        SetActiveTransitionProperty(activeTransition, property);
    }

    SetActiveTransitionSourceValue(activeTransition, GetPropertyValue(property, entity));
    SetActiveTransitionDestinationValue(activeTransition, destinationValue);
    SetActiveTransitionTime(activeTransition, 0.0f);
    SetActiveTransitionDuration(activeTransition, time);
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

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnUpdateTransitions, AppLoopOf_Transition())

    SetAppLoopOrder(AppLoopOf_Transition(), AppLoopOrder_Update);
    SetStopWatchRunning(StopWatchOf_Transition(), true);
EndUnit()