//
// Created by Kim on 12/05/2018.
//

#ifndef PLAZA_INVOCATION_H
#define PLAZA_INVOCATION_H

#include <Core/Entity.h>
#include <Core/Variant.h>
#include <Core/Function.h>

DeclareComponent(Invocation)
DeclareComponentPropertyReactive(Invocation, Function, InvocationFunction)
DeclareComponentPropertyReactive(Invocation, Variant, InvocationResult)

DeclareComponent(InvocationArgument)
DeclareComponentPropertyReactive(InvocationArgument, Variant, InvocationArgumentValue);

bool Invoke(Entity invocation);

#endif //PLAZA_INVOCATION_H
