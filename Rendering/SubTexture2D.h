//
// Created by Kim Johannsen on 07-03-2018.
//

#ifndef PLAZA_SUBTEXTURE2D_H
#define PLAZA_SUBTEXTURE2D_H

#include <Core/Entity.h>

DeclareComponent(SubTexture2D)
DeclareService(SubTexture2D)

DeclareComponentPropertyReactive(SubTexture2D, v2i, SubTexture2DOffset)
DeclareComponentPropertyReactive(SubTexture2D, v2i, SubTexture2DSize)
DeclareComponentPropertyReactive(SubTexture2D, Entity, SubTexture2DTexture)

Entity GetSubTexture2DUvOffsetScaleUniform();

#endif //PLAZA_SUBTEXTURE2D_H
