//
// Created by Kim on 10/07/2018.
//

#ifndef PLAZA_JAVASCRIPTCONTEXT_H
#define PLAZA_JAVASCRIPTCONTEXT_H

#include <Core/NativeUtils.h>

Unit(JavaScriptContext)
    Component(JavaScriptContext)
    Node(JavaScriptMainContext)

bool EvaluateJavaScript(Entity context, StringRef name, StringRef code);

#endif //PLAZA_JAVASCRIPTCONTEXT_H
