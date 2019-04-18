//
// Created by Kim on 12/05/2018.
//

#ifndef PLAZA_INVOCATION_H
#define PLAZA_INVOCATION_H

#include <Core/NativeUtils.h>

struct FunctionInvocation {
    Entity InvocationFunction;
    Variant InvocationResult;
    ChildArray InvocationArguments;
};

struct PropertyInvocation {
    Entity InvocationProperty, InvocationTarget;
    Variant InvocationValue;
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

    Component(FunctionInvocation)
        ReferenceProperty(Function, InvocationFunction)
        Property(Variant, InvocationResult)
        ArrayProperty(InvocationArgument, InvocationArguments)

    Component(PropertyInvocation)
        ReferenceProperty(Property, InvocationProperty)
        Property(Entity, InvocationTarget)
        Property(Variant, InvocationValue)

    Component(InvocationToggle)
        Property(bool, InvocationToggleState)

Function(Invoke, bool, Entity invocation);

#endif //PLAZA_INVOCATION_H
