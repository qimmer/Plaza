//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/String.h>
#include "Uniform.h"

struct Uniform {
    Uniform() : UniformArrayCount(1), UniformName(""), UniformType(0) {}

    String UniformName;
    u32 UniformArrayCount;
    Type UniformType;
};

BeginUnit(Uniform)
    BeginComponent(Uniform)
    RegisterProperty(StringRef, UniformName)
    RegisterProperty(u32, UniformArrayCount)
    RegisterProperty(Type, UniformType)
EndComponent()

RegisterProperty(StringRef, UniformName)
RegisterProperty(u32, UniformArrayCount)
RegisterProperty(Type, UniformType)

