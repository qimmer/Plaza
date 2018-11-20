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
    Variant result;

    u32 count = 0;
    auto invocationArguments = GetInvocationArguments(invocationEntity, &count);
    auto invocation = GetInvocationData(invocationEntity);

    if(!invocation) return false;

    auto arguments = (Variant*)alloca(sizeof(Variant) * count);
    for(int i = 0; i < count; ++i) {
        auto argument = GetInvocationArgumentData(invocationArguments[i]);

        arguments[i] = argument->InvocationArgumentValue;
    }

    if(HasComponent(invocation->InvocationFunction, ComponentOf_Function())) {
        result = CallFunction(
                invocation->InvocationFunction,
                count,
                arguments
        );

        if(IsEntityValid(invocationEntity)) {
            SetInvocationResult(invocationEntity, result);
        }

        return true;
    } else if (HasComponent(invocation->InvocationFunction, ComponentOf_Event())) {
        FireEventFast(
                invocation->InvocationFunction,
                count,
                arguments
        );

        result.type = TypeOf_unknown;
        SetInvocationResult(invocationEntity, result);

        return true;
    }

    Log(invocationEntity, LogSeverity_Error, "Function invocation '%s' failed: Invocation function is either a function or an event.", GetName(invocationEntity));

    return false;
}

LocalFunction(OnInvocationToggled, void, Entity invocation, bool oldState, bool newState) {
    if(newState) {
        Invoke(invocation);
    }
}

BeginUnit(Invocation)
    BeginComponent(Invocation)
        RegisterReferenceProperty(Function, InvocationFunction)
        RegisterProperty(Variant, InvocationResult)
        RegisterArrayProperty(InvocationArgument, InvocationArguments)
    EndComponent()
    BeginComponent(InvocationArgument)
        RegisterProperty(Variant, InvocationArgumentValue)
    EndComponent()
    BeginComponent(InvocationToggle)
        RegisterProperty(bool, InvocationToggleState)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InvocationToggleState()), OnInvocationToggled, 0)
EndUnit()
