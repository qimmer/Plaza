//
// Created by Kim on 12/05/2018.
//

#include "Invocation.h"
#include <Core/Variant.h>
#include <Core/Function.h>
#include <Core/Debug.h>
#include <Core/Vector.h>
#include <Core/Identification.h>

#define ARGUMENT_DATA_MAX 4096

API_EXPORT bool Invoke(Entity invocationEntity) {
    auto functionInvocation = GetFunctionInvocation(invocationEntity);
    auto propertyInvocation = GetPropertyInvocation(invocationEntity);

    if(propertyInvocation.InvocationProperty) {
        SetPropertyValue(propertyInvocation.InvocationProperty, propertyInvocation.InvocationTarget, propertyInvocation.InvocationValue);
        return true;
    }

    if(functionInvocation.InvocationFunction) {
        auto arguments = (Variant*)alloca(sizeof(Variant) * functionInvocation.InvocationArguments.GetSize());
        for(int i = 0; i < functionInvocation.InvocationArguments.GetSize(); ++i) {
            arguments[i] = GetInvocationArgument(functionInvocation.InvocationArguments[i]).InvocationArgumentValue;
        }

        if(HasComponent(functionInvocation.InvocationFunction, ComponentOf_Function())) {
            functionInvocation.InvocationResult = CallFunction(
                    functionInvocation.InvocationFunction,
                    functionInvocation.InvocationArguments.GetSize(),
                    arguments
            );

            if(IsEntityValid(invocationEntity)) {
                SetFunctionInvocation(invocationEntity, functionInvocation);
            }

            return true;
        }

        Log(invocationEntity, LogSeverity_Error, "Invocation '%s' failed: Invocation function is neither a function or an event.", GetIdentification(invocationEntity).Uuid);
        return false;
    }

    return false;
}

static void OnInvocationToggleChanged(Entity invocation, const InvocationToggle& oldData, const InvocationToggle& newData) {
    if(newData.InvocationToggleState) {
        Invoke(invocation);
    }
}

BeginUnit(Invocation)
    BeginComponent(FunctionInvocation)
        RegisterReferenceProperty(Function, InvocationFunction)
        RegisterProperty(Variant, InvocationResult)
        RegisterArrayProperty(InvocationArgument, InvocationArguments)
    EndComponent()

    BeginComponent(PropertyInvocation)
        RegisterReferenceProperty(Property, InvocationProperty)
        RegisterProperty(Variant, InvocationValue)
        RegisterProperty(Entity, InvocationTarget)
    EndComponent()

    BeginComponent(InvocationArgument)
        RegisterProperty(Variant, InvocationArgumentValue)
    EndComponent()
    BeginComponent(InvocationToggle)
        RegisterProperty(bool, InvocationToggleState)
    EndComponent()

    RegisterSystem(OnInvocationToggleChanged, ComponentOf_InvocationToggle())
EndUnit()
