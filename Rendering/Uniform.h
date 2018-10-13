//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_UNIFORM_H
#define PLAZA_UNIFORM_H

#include <Core/NativeUtils.h>

Unit(Uniform)
    Component(Uniform)
        Property(u8, UniformArrayCount)
        Property(Type, UniformType)
        Property(StringRef, UniformIdentifier)
        ReferenceProperty(Property, UniformEntityProperty)
        ReferenceProperty(Property, UniformElementProperty)

#endif //PLAZA_UNIFORM_H
