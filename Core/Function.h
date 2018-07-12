#ifndef PLAZA_FUNCTION_H
#define PLAZA_FUNCTION_H

#include <Core/NativeUtils.h>
#include "Variant.h"

typedef bool(*FunctionCaller)(
    u64 functionImplementation,
    Type returnArgumentTypeIndex,
    void *returnData,
    u32 numArguments,
    const Type *argumentTypes,
    const void **argumentDataPtrs
);

Unit(Function)

Component(Function)
    Property(u64, FunctionImplementation)
    Property(Entity, FunctionReturnType)
    Property(FunctionCaller, FunctionCaller)

Component(FunctionArgument)
    Property(Entity, FunctionArgumentType)

Function FindFunctionByName(StringRef name);
void SetFunctionArgsByDecl(Entity f, StringRef argsDecl);

bool CallFunction(
    Entity f,
    void *returnData,
    u32 numArguments,
    const Type *argumentTypes,
    const void **argumentDataPtrs
);

bool CallNativeFunction(
    u64 funcPtr,
    Type returnArgumentTypeIndex,
    void *returnData,
    u32 numArguments,
    const Type *argumentTypes,
    const void **argumentDataPtrs
);

u32 GetFunctionArguments(u32 functionIndex, u32 maxArguments, Type *argumentTypes);
Type GetFunctionReturnTypeByIndex(u32 functionIndex);

void __InitializeFunction();
int __ArgStackOffset(int value);

#endif