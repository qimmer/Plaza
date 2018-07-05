//
// Created by Kim on 12/05/2018.
//

#include "Invocation.h"
#include <Core/Variant.h>
#include <Core/Function.h>

#define ARGUMENT_DATA_MAX 4096

struct Invocation {
    Function InvocationFunction;
    Variant InvocationResult;
};

DefineComponent(Invocation)
    DefineProperty(Function, InvocationFunction)
EndComponent()

DefineComponentPropertyReactive(Invocation, Function, InvocationFunction)
DefineComponentPropertyReactive(Invocation, Variant, InvocationResult)

struct InvocationArgument {
    Variant InvocationArgumentValue;
};

DefineComponent(InvocationArgument)
    DefineProperty(Variant, InvocationArgumentValue)
EndComponent()

DefineComponentPropertyReactive(InvocationArgument, Variant, InvocationArgumentValue);

API_EXPORT bool Invoke(Entity invocationEntity) {
    char argumentBuffer[ARGUMENT_DATA_MAX];
    char returnBuffer[ARGUMENT_DATA_MAX];

    char *argumentPtr = argumentBuffer;
    char *argumentMax = argumentPtr + ARGUMENT_DATA_MAX - sizeof(Variant);

    auto invocation = GetInvocation(invocationEntity);

    if(!invocation) return false;

    for_children(argumentEntity, invocationEntity) {
        auto argument = GetInvocationArgument(argumentEntity);

        if(!argument) continue;

        if(argumentPtr >= argumentMax) {
            Log(LogChannel_Core, LogSeverity_Error, "Function invocation '%s' failed: Too many arguments.", GetName(invocationEntity));
            return false;
        }

        u32 writtenBytes = 0;
        if(!GetVariant(argument->InvocationArgumentValue, argumentMax - argumentPtr, argumentPtr, &writtenBytes)) {
            Log(LogChannel_Core, LogSeverity_Error, "Function invocation '%s' failed: Could not extract variant.", GetName(invocationEntity));
            return false;
        }

        argumentPtr += writtenBytes;
    }

    CallFunction(invocation->InvocationFunction, argumentBuffer, returnBuffer);

    auto returnType = GetFunctionReturnType(invocation->InvocationFunction);
    SetInvocationResult(invocationEntity, CreateVariant(returnType, ARGUMENT_DATA_MAX, returnBuffer));

    return true;
}
