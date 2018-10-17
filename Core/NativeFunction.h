//
// Created by Kim on 17-10-2018.
//

#ifndef PLAZA_FUNCTIONINVOKERS_H
#define PLAZA_FUNCTIONINVOKERS_H

#include "NativeUtils.h"

typedef bool(*NativeFunctionInvokerType)(
        void *funcPtr,
        void *returnData,
        const void **argumentDataPtrs
);

struct NativeFunction {
    NativePtr NativeFunctionPointer;
    NativePtr NativeFunctionInvoker;
};

Unit(NativeFunction)
    Component(NativeFunction)
        __PropertyCore(NativeFunction, NativePtr, NativeFunctionPointer)
        __PropertyCore(NativeFunction, NativePtr, NativeFunctionInvoker)

void RegisterFunctionSignatureImpl(NativePtr invokerFuncPtr, StringRef signature);
void SetNativeFunctionInvokerBySignature(Entity function, StringRef signature);

#endif //PLAZA_FUNCTIONINVOKERS_H
