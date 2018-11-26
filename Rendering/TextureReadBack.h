//
// Created by Kim on 30-10-2018.
//

#ifndef PLAZA_TEXTUREREADBACK_H
#define PLAZA_TEXTUREREADBACK_H

#include <Core/NativeUtils.h>

struct TextureReadBack {
    Entity TextureReadBackSourceTexture, TextureReadBackBlitTexture, TextureReadBackBuffer;
    u32 TextureReadBackPendingReadBacks;
};

Unit(TextureReadBack)
    Component(TextureReadBack)
        ReferenceProperty(Texture2D, TextureReadBackSourceTexture)
        ChildProperty(Texture2D, TextureReadBackBlitTexture)
        ChildProperty(MemoryStream, TextureReadBackBuffer)
        Property(u32, TextureReadBackPendingReadBacks)

#endif //PLAZA_TEXTUREREADBACK_H
