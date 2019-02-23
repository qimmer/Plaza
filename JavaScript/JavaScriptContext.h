//
// Created by Kim on 10/07/2018.
//

#ifndef PLAZA_JAVASCRIPTCONTEXT_H
#define PLAZA_JAVASCRIPTCONTEXT_H

#include <Core/NativeUtils.h>

Unit(JavaScriptContext)
    Component(JavaScriptContext)

    Component(JavaScript)
        ArrayProperty(JavaScriptFunction, JavaScriptFunctions)

    Component(JavaScriptFunction)
        Property(NativePtr, JavaScriptFunctionHandle)
        Property(NativePtr, JavaScriptFunctionContext)

    Declare(JavaScriptContext, JavaScriptMainContext)

#endif //PLAZA_JAVASCRIPTCONTEXT_H
