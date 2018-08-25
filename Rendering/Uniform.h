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

    Component(UniformStateElement)
        Property(m4x4f, UniformStateElementMat4);
        Property(v4f, UniformStateElementVec4);
        ReferenceProperty(Texture, UniformStateElementTexture);

    Component(UniformState)
        ReferenceProperty(Uniform, UniformStateUniform);
        ArrayProperty(UniformStateElement, UniformStateElements)

#endif //PLAZA_UNIFORM_H
