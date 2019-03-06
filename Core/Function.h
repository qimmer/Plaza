#ifndef PLAZA_FUNCTION_H
#define PLAZA_FUNCTION_H

#include <Core/NativeUtils.h>
#include "Variant.h"

typedef Variant(*FunctionCallerType)(
	NativePtr functionPtr,
    const Variant *arguments
);

struct FunctionArgument {
    Type FunctionArgumentType;
};

struct Function {
	NativePtr FunctionCaller, FunctionPtr;
	u32 NumFunctionArguments;
    Type FunctionReturnType;
	Type FunctionArgumentTypes[11];
};

Unit(Function)
    Component(FunctionArgument)
		__PropertyCore(Function, Type, FunctionArgumentType)

    Component(Function)
        __PropertyCore(Function, NativePtr, FunctionCaller)
		__PropertyCore(Function, NativePtr, FunctionPtr)
		__PropertyCore(Function, Type, FunctionReturnType)
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