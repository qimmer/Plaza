//
// Created by Kim Johannsen on 07-03-2018.
//

#ifndef PLAZA_SUBTEXTURE2D_H
#define PLAZA_SUBTEXTURE2D_H

#include <Core/Entity.h>

DeclareComponent(SubTexture2D)
DeclareService(SubTexture2D)

DeclareComponentProperty(SubTexture2D, v2i, SubTexture2DOffset)
DeclareComponentProperty(SubTexture2D, v2i, SubTexture2DSize)

Entity GetSubTexture2DUvOffsetScaleUniform();

#endif //PLAZA_SUBTEXTURE2D_H
