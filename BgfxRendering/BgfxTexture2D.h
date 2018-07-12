//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXTEXTURE2D_H
#define PLAZA_BGFXTEXTURE2D_H

#include <Core/NativeUtils.h>


    Unit(BgfxTexture2D)
    Component(BgfxTexture2D)
    DeclareService(BgfxTexture2D)

    u16 GetBgfxTexture2DHandle(Entity entity);

    void UpdateBgfxTexture2D(Entity entity);


#endif //PLAZA_BGFXTEXTURE2D_H
