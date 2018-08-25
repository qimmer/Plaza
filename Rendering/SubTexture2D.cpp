//
// Created by Kim Johannsen on 07-03-2018.
//

#include "SubTexture2D.h"
#include "Uniform.h"

struct SubTexture2D {
    v2i SubTexture2DOffset, SubTexture2DSize;
    Entity SubTexture2DTexture;
};

BeginUnit(SubTexture2D)
    BeginComponent(SubTexture2D)
        RegisterProperty(v2i, SubTexture2DOffset)
        RegisterProperty(v2i, SubTexture2DSize)
        RegisterProperty(Entity, SubTexture2DTexture)
    EndComponent()
EndUnit()
