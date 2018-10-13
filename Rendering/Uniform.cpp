//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Uniform.h"

struct Uniform {
    u8 UniformArrayCount;
    Type UniformType;
    char UniformIdentifier[128];
    Entity UniformEntityProperty, UniformElementProperty;
};

BeginUnit(Uniform)
    BeginComponent(Uniform)
        RegisterProperty(u8, UniformArrayCount)
        RegisterProperty(Type, UniformType)
        RegisterProperty(StringRef, UniformIdentifier)
        RegisterReferenceProperty(Property, UniformEntityProperty)
        RegisterReferenceProperty(Property, UniformElementProperty)
    EndComponent()
EndUnit()

