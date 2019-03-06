//
// Created by Kim on 17-10-2018.
//

#ifndef PLAZA_FUNCTIONINVOKERS_H
#define PLAZA_FUNCTIONINVOKERS_H

#include "NativeUtils.h"

void RegisterFunctionSignatureImpl(NativePtr invokerFuncPtr, StringRef signature);
void SetNativeFunctionInvokerBySignature(Entity function, StringRef signature);

#endif //PLAZA_FUNCTIONINVOKERS_H
