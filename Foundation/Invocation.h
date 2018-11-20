//
// Created by Kim on 12/05/2018.
//

#ifndef PLAZA_INVOCATION_H
#define PLAZA_INVOCATION_H

#include <Core/NativeUtils.h>

struct Invocation {
    Entity InvocationFunction;
    Variant InvocationResult;
};

struct InvocationArgument {
    Variant InvocationArgumentValue;
};

struct InvocationToggle {
    bool InvocationToggleState;
};

Unit(Invocation)
    Component(InvocationArgument)
        Property(Variant, InvocationArgumentValue);

    Component(Invocation)
        ReferenceProperty(Function, InvocationFunction)
        Property(Variant, InvocationResult)
        ArrayProperty(InvocationArgument, InvocationArguments)

    Component(InvocationToggle)
        Property(bool, InvocationToggleState)

Function(Invoke, bool, Entity invocation);

#endif //PLAZA_INVOCATION_H
