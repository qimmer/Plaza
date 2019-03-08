//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Texture2D.h"
#include "Texture.h"

BeginUnit(Texture2D)
    BeginComponent(Texture2D)
        RegisterProperty(v2i, TextureSize2D)
    EndComponent()

    BeginComponent(SubTexture2D)
        RegisterProperty(v2i, SubTexture2DOffset)
        RegisterProperty(v2i, SubTexture2DSize)
    EndComponent()

    BeginComponent(TextureAtlas)
        RegisterBase(Texture2D)
        RegisterArrayProperty(SubTexture2D, TextureAtlasSubTextures)
    EndComponent()
EndUnit()
