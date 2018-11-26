//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_UNIFORM_H
#define PLAZA_UNIFORM_H

#include <Core/NativeUtils.h>

struct Uniform {
    Entity UniformEntityProperty, UniformElementProperty;
    StringRef UniformIdentifier;
    u8 UniformArrayCount, UniformSamplerIndex;
};

Unit(Uniform)
    Enum(UniformSource)

    Component(Uniform)
        Property(u8, UniformArrayCount)
        Property(u8, UniformSamplerIndex)
        Property(StringRef, UniformIdentifier)
        ReferenceProperty(Property, UniformEntityProperty)
        ReferenceProperty(Property, UniformElementProperty)

#endif //PLAZA_UNIFORM_H
