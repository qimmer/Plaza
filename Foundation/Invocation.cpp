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

    Vector(InvocationArguments, Entity, 16)
};

struct InvocationArgument {
    Variant InvocationArgumentValue;
};

API_EXPORT bool Invoke(Entity invocationEntity) {
    const void *argumentPtrs[16];
    u8 argumentTypeIndices[16];
    Variant result;

    auto invocation = GetInvocationData(invocationEntity);

    if(!invocation) return false;

    if(invocation->InvocationArguments.Count > 16) {
        Log(invocationEntity, LogSeverity_Error, "Function invocation '%s' failed: Too many arguments.", GetName(invocationEntity));
        return false;
    }

    for(int i = 0; i < invocation->InvocationArguments.Count; ++i) {
        auto argument = GetInvocationArgumentData(GetVector(invocation->InvocationArguments)[i]);

        argumentPtrs[i] = argument->InvocationArgumentValue.buffer;
        argumentTypeIndices[i] = argument->InvocationArgumentValue.type;
    }

    if(HasComponent(invocation->InvocationFunction, ComponentOf_Function())) {
        CallFunction(
                invocation->InvocationFunction,
                result.buffer,
                invocation->InvocationArguments.Count,
                argumentTypeIndices,
                argumentPtrs
        );

        result.type = GetFunctionReturnType(invocation->InvocationFunction);
        SetInvocationResult(invocationEntity, result);

        return true;
    } else if (HasComponent(invocation->InvocationFunction, ComponentOf_Event())) {
        FireEventFast(
                invocation->InvocationFunction,
                invocation->InvocationArguments.Count,
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
