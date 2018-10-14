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
    const void *argumentPtrs[16];
    u8 argumentTypeIndices[16];
    Variant result;

    u32 count = 0;
    auto invocationArguments = GetInvocationArguments(invocationEntity, &count);
    auto invocation = GetInvocationData(invocationEntity);

    if(!invocationArguments) return false;

    if(count > 16) {
        Log(invocationEntity, LogSeverity_Error, "Function invocation '%s' failed: Too many arguments.", GetName(invocationEntity));
        return false;
    }

    for(int i = 0; i < count; ++i) {
        auto argument = GetInvocationArgumentData(invocationArguments[i]);

        argumentPtrs[i] = &argument->InvocationArgumentValue.data;
        argumentTypeIndices[i] = argument->InvocationArgumentValue.type;
    }

    if(HasComponent(invocation->InvocationFunction, ComponentOf_Function())) {
        CallFunction(
                invocation->InvocationFunction,
                &result.data,
                count,
                argumentTypeIndices,
                argumentPtrs
        );

        result.type = GetFunctionReturnType(invocation->InvocationFunction);
        SetInvocationResult(invocationEntity, result);

        return true;
    } else if (HasComponent(invocation->InvocationFunction, ComponentOf_Event())) {
        FireEventFast(
                invocation->InvocationFunction,
                count,
                argumentTypeIndices,
                argumentPtrs
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
