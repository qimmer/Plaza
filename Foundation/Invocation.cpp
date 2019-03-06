//
// Created by Kim on 12/05/2018.
//

#include "Invocation.h"
#include <Core/Variant.h>
#include <Core/Function.h>
#include <Core/Debug.h>
#include <Core/Vector.h>
#include <Core/Identification.h>

#define ARGUMENT_DATA_MAX 4096

API_EXPORT bool Invoke(Entity invocationEntity) {
    auto functionInvocation = GetFunctionInvocationData(invocationEntity);
    auto propertyInvocation = GetPropertyInvocationData(invocationEntity);

    if(propertyInvocation) {
        SetPropertyValue(propertyInvocation->InvocationProperty, propertyInvocation->InvocationTarget, propertyInvocation->InvocationValue);
        return true;
    }

    if(functionInvocation) {
        Variant result;
        u32 count = 0;

        auto invocationArguments = GetInvocationArguments(invocationEntity, &count);

        auto arguments = (Variant*)alloca(sizeof(Variant) * count);
        for(int i = 0; i < count; ++i) {
            auto argument = GetInvocationArgumentData(invocationArguments[i]);

            arguments[i] = argument->InvocationArgumentValue;
        }

        if(HasComponent(functionInvocation->InvocationFunction, ComponentOf_Function())) {
            result = CallFunction(
                    functionInvocation->InvocationFunction,
                    count,
                    arguments
            );

            if(IsEntityValid(invocationEntity)) {
                SetInvocationResult(invocationEntity, result);
            }

            return true;
        } else if (HasComponent(functionInvocation->InvocationFunction, ComponentOf_Event())) {
            FireEventFast(
                    functionInvocation->InvocationFunction,
                    count,
                    arguments
            );

            result.type = TypeOf_unknown;
            SetInvocationResult(invocationEntity, result);

            return true;
        }

        Log(invocationEntity, LogSeverity_Error, "Invocation '%s' failed: Invocation function is neither a function or an event.", GetUuid(invocationEntity));
        return false;
    }

    return false;
}

LocalFunction(OnInvocationToggled, void, Entity invocation, bool oldState, bool newState) {
    if(newState) {
        Invoke(invocation);
    }
}

BeginUnit(Invocation)
    BeginComponent(FunctionInvocation)
        RegisterReferenceProperty(Function, InvocationFunction)
        RegisterProperty(Variant, InvocationResult)
        RegisterArrayProperty(InvocationArgument, InvocationArguments)
    EndComponent()

    BeginComponent(PropertyInvocation)
        RegisterReferenceProperty(Property, InvocationProperty)
        RegisterProperty(Variant, InvocationValue)
        RegisterProperty(Entity, InvocationTarget)
    EndComponent()

    BeginComponent(InvocationArgument)
        RegisterProperty(Variant, InvocationArgumentValue)
    EndComponent()
    BeginComponent(InvocationToggle)
        RegisterProperty(bool, InvocationToggleState)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InvocationToggleState()), OnInvocationToggled, 0)
EndUnit()
