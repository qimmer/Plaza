//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_UNIFORM_H
#define PLAZA_UNIFORM_H

#include <Core/NativeUtils.h>

Unit(Uniform)
    Component(Uniform)

        Property(StringRef, UniformName)
        Property(u32, UniformArrayCount)
        Property(Type, UniformType)

#endif //PLAZA_UNIFORM_H
