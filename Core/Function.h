#ifndef PLAZA_FUNCTION_H
#define PLAZA_FUNCTION_H

#include <Core/NativeUtils.h>
#include "Variant.h"

typedef Variant(*FunctionCallerType)(
    Entity entity,
    u32 numArguments,
    const Variant *arguments
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

Variant CallFunction(
    Entity f,
    u32 numArguments,
    const Variant *arguments
);

void __InitializeFunction();
int __ArgStackOffset(int value);

#endif