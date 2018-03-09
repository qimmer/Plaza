//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Uniform.h"

struct Uniform {
    Uniform() : UniformArrayCount(1) {}

    StringRef UniformName;
    u32 UniformArrayCount;
    Type UniformType;
};

DefineComponent(Uniform)
    DefineProperty(StringRef, UniformName)
    DefineProperty(u32, UniformArrayCount)
    DefineProperty(Type, UniformType)
EndComponent()

DefineComponentPropertyReactive(Uniform, StringRef, UniformName)
DefineComponentPropertyReactive(Uniform, u32, UniformArrayCount)
DefineComponentPropertyReactive(Uniform, Type, UniformType)

