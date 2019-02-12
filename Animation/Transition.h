//
// Created by Kim on 08-01-2019.
//

#ifndef PLAZA_TRANSITION_H
#define PLAZA_TRANSITION_H

#include <Core/NativeUtils.h>

struct TransitioningEntity {
};

struct ActiveTransition {
    Entity ActiveTransitionProperty;
    Variant ActiveTransitionSourceValue;
    Variant ActiveTransitionDestinationValue;
    float ActiveTransitionDuration, ActiveTransitionTime;
};

Unit(Transition)
    Component(TransitioningEntity)
        ArrayProperty(Transition, ActiveTransitions)

    Component(ActiveTransition)
        Property(float, ActiveTransitionDuration)
        Property(float, ActiveTransitionTime)
        Property(Variant, ActiveTransitionSourceValue)
        Property(Variant, ActiveTransitionDestinationValue)
        ReferenceProperty(Property, ActiveTransitionProperty)

    Declare(AppLoop, Transition)
    Declare(StopWatch, Transition)

    Function(Transition, void, Entity entity, Entity property, Variant destinationValue, float time)

#endif //PLAZA_TRANSITION_H
