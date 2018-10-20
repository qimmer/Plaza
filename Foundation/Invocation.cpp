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

struct Invocation {
    Entity InvocationFunction;
    Variant InvocationResult;
};

struct InvocationArgument {
    Variant InvocationArgumentValue;
};

API_EXPORT bool Invoke(Entity invocationEntity) {
    Variant result;

    u32 count = 0;
    auto invocationArguments = GetInvocationArguments(invocationEntity, &count);
    auto invocation = GetInvocationData(invocationEntity);

    if(!invocationArguments) return false;

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

        SetInvocationResult(invocationEntity, result);

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

BeginUnit(Invocation)
    BeginComponent(Invocation)
        RegisterProperty(Entity, InvocationFunction)
        RegisterProperty(Variant, InvocationResult)
        RegisterArrayProperty(InvocationArgument, InvocationArguments)
    EndComponent()
    BeginComponent(InvocationArgument)
        RegisterProperty(Variant, InvocationArgumentValue)
    EndComponent()
EndUnit()
