//
// Created by Kim Johannsen on 07-03-2018.
//

#ifndef PLAZA_SUBTEXTURE2D_H
#define PLAZA_SUBTEXTURE2D_H

#include <Core/NativeUtils.h>

Unit(SubTexture2D)
    Component(SubTexture2D)

        Property(v2i, SubTexture2DOffset)
        Property(v2i, SubTexture2DSize)
        Property(Entity, SubTexture2DTexture)

Entity GetSubTexture2DUvOffsetScaleUniform();

#endif //PLAZA_SUBTEXTURE2D_H
