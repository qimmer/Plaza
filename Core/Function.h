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

struct FunctionArgument {
    Type FunctionArgumentType;
};

struct Function {
    Type FunctionReturnType;
    NativePtr FunctionCaller;
};

Unit(Function)
    Component(FunctionArgument)
        Property(Type, FunctionArgumentType)

    Component(Function)
        __PropertyCore(Function, NativePtr, FunctionCaller)
        Property(Type, FunctionReturnType)
        ArrayProperty(FunctionArgument, FunctionArguments)

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