//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Uniform.h"

struct Uniform {
    Entity UniformEntityProperty, UniformElementProperty;
    StringRef UniformIdentifier;
    u8 UniformArrayCount, UniformSource;
    Type UniformType;
};

BeginUnit(Uniform)
    BeginEnum(UniformSource, false)
        RegisterFlag(UniformSource_Unknown)
        RegisterFlag(UniformSource_Scene)
        RegisterFlag(UniformSource_Camera)
        RegisterFlag(UniformSource_Light)
        RegisterFlag(UniformSource_Material)
        RegisterFlag(UniformSource_Renderable)
    EndEnum()
    BeginComponent(Uniform)
        RegisterPropertyEnum(u8, UniformSource, UniformSource)
        RegisterProperty(u8, UniformArrayCount)
        RegisterProperty(Type, UniformType)
        RegisterProperty(StringRef, UniformIdentifier)
        RegisterReferenceProperty(Property, UniformEntityProperty)
        RegisterReferenceProperty(Property, UniformElementProperty)
    EndComponent()
EndUnit()

