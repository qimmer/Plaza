//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Uniform.h"

BeginUnit(Uniform)
    BeginComponent(Uniform)
        RegisterProperty(u8, UniformArrayCount)
        RegisterProperty(u8, UniformSamplerIndex)
        RegisterProperty(Type, UniformType)
        RegisterProperty(StringRef, UniformIdentifier)
        RegisterReferenceProperty(Property, UniformEntityProperty)
        RegisterReferenceProperty(Property, UniformElementProperty)
    EndComponent()
EndUnit()

