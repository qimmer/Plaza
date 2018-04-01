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

DefineComponent(Uniform)
    DefinePropertyReactive(StringRef, UniformName)
    DefinePropertyReactive(u32, UniformArrayCount)
    DefinePropertyReactive(Type, UniformType)
EndComponent()

DefineComponentPropertyReactive(Uniform, StringRef, UniformName)
DefineComponentPropertyReactive(Uniform, u32, UniformArrayCount)
DefineComponentPropertyReactive(Uniform, Type, UniformType)

