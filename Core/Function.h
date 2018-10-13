#ifndef PLAZA_FUNCTION_H
#define PLAZA_FUNCTION_H

#include <Core/NativeUtils.h>
#include "Variant.h"

typedef bool(*FunctionCallerType)(
    Entity entity,
    void *returnData,
    u32 numArguments,
    const Type *argumentTypes,
    const void **argumentDataPtrs
);

Unit(Function)
    Component(FunctionArgument)
        Property(Entity, FunctionArgumentType)

    Component(Function)
        Property(Entity, FunctionReturnType)
        ArrayProperty(FunctionArgument, FunctionArguments)

    Component(NativeFunction)

Function(FindFunctionByName, Entity, StringRef name)
FunctionCallerType GetFunctionCaller(Entity function);

void SetFunctionCaller(Entity function, FunctionCallerType caller);
void SetNativeFunctionPointer(Entity function, void *ptr);

void SetFunctionArgsByDecl(Entity f, StringRef argsDecl);

void ProfileStart(StringRef tag, double thresholdMsecs);
void ProfileEnd();

bool CallFunction(
    Entity f,
    void *returnData,
    u32 numArguments,
    const Type *argumentTypes,
    const void **argumentDataPtrs
);

void __InitializeFunction();
int __ArgStackOffset(int value);

#endif