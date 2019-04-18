//
// Created by Kim on 10/07/2018.
//

#ifndef PLAZA_JAVASCRIPTCONTEXT_H
#define PLAZA_JAVASCRIPTCONTEXT_H

#include <Core/NativeUtils.h>

struct JavaScriptContext {
    NativePtr JavaScriptContextHandle;
};

struct JavaScriptFunction {
    NativePtr JavaScriptFunctionHandle, JavaScriptFunctionContext;
};

struct JavaScript {
    ChildArray JavaScriptFunctions;
};

Unit(JavaScriptContext)
    Component(JavaScriptContext)
        Property(NativePtr, JavaScriptContextHandle)

    Component(JavaScript)
        ArrayProperty(JavaScriptFunction, JavaScriptFunctions)

    Component(JavaScriptFunction)
        Property(NativePtr, JavaScriptFunctionHandle)
        Property(NativePtr, JavaScriptFunctionContext)

    Declare(JavaScriptContext, JavaScriptMainContext)

#endif //PLAZA_JAVASCRIPTCONTEXT_H
