//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Uniform.h"

struct Uniform {
    u8 UniformArrayCount;
    Type UniformType;
    char UniformIdentifier[128];
};

struct UniformState {
    Vector(UniformStateElements, Entity, 256)
    Entity UniformStateUniform;
};

struct UniformStateElement {
    union {
        m4x4f UniformStateElementMat4;
        v4f UniformStateElementVec4;
        Entity UniformStateElementTexture;
    };
};

BeginUnit(Uniform)
    BeginComponent(Uniform)
        RegisterProperty(u8, UniformArrayCount)
        RegisterProperty(Type, UniformType)
        RegisterProperty(StringRef, UniformIdentifier)
    EndComponent()

    BeginComponent(UniformState)
        RegisterProperty(Entity, UniformStateUniform)
        RegisterArrayProperty(UniformStateElement, UniformStateElements)
    EndComponent()

    BeginComponent(UniformStateElement)
        RegisterProperty(v4f, UniformStateElementVec4)
        RegisterProperty(m4x4f, UniformStateElementMat4)
        RegisterProperty(Entity, UniformStateElementTexture)
    EndComponent()
EndUnit()

