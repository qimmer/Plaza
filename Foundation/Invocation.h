//
// Created by Kim on 12/05/2018.
//

#ifndef PLAZA_INVOCATION_H
#define PLAZA_INVOCATION_H

#include <Core/NativeUtils.h>

Unit(Invocation)
    Component(InvocationArgument)
        Property(Variant, InvocationArgumentValue);

    Component(Invocation)
        Property(Entity, InvocationFunction)
        Property(Variant, InvocationResult)
        ArrayProperty(InvocationArgument, InvocationArguments)

Function(Invoke, bool, Entity invocation);

#endif //PLAZA_INVOCATION_H
