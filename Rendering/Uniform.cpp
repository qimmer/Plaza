//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Invalidation.h>
#include "Uniform.h"


    struct Uniform {
        Uniform() {
            memset(this, 0, sizeof(Uniform));
        }
        StringRef UniformName;
        u32 UniformArrayCount;
        Type UniformType;
    };

    DefineComponent(Uniform)
        Dependency(Invalidation)
    EndComponent()

    DefineComponentProperty(Uniform, StringRef, UniformName)
    DefineComponentProperty(Uniform, u32, UniformArrayCount)
    DefineComponentProperty(Uniform, Type, UniformType)

