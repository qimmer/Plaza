//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_UNIFORM_H
#define PLAZA_UNIFORM_H

#include <Core/NativeUtils.h>

enum {
    UniformSource_Unknown,
    UniformSource_Scene,
    UniformSource_Camera,
    UniformSource_Light,
    UniformSource_Renderable,
    UniformSource_Material
};

Unit(Uniform)
    Enum(UniformSource)

    Component(Uniform)
        Property(u8, UniformArrayCount)
        Property(Type, UniformType)
        Property(StringRef, UniformIdentifier)
        Property(u8, UniformSource)
        ReferenceProperty(Property, UniformEntityProperty)
        ReferenceProperty(Property, UniformElementProperty)

#endif //PLAZA_UNIFORM_H
