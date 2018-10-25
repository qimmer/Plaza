//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_TEXTURE2D_H
#define PLAZA_TEXTURE2D_H

#include <Core/NativeUtils.h>

struct Texture2D {
    v2i TextureSize2D;
};

struct TextureAtlas {
};

struct SubTexture2D {
    v2i SubTexture2DOffset, SubTexture2DSize;
};

Unit(Texture2D)
    Component(Texture2D)
        Property(v2i, TextureSize2D)

    Component(TextureAtlas)
        ArrayProperty(SubTexture2D, TextureAtlasSubTextures)

    Component(SubTexture2D)
        Property(v2i, SubTexture2DOffset)
        Property(v2i, SubTexture2DSize)


#endif //PLAZA_TEXTURE2D_H
