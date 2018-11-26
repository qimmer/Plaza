//
// Created by Kim on 30-10-2018.
//

#ifndef PLAZA_BGFXTEXTUREREADBACK_H
#define PLAZA_BGFXTEXTUREREADBACK_H

#include <Core/NativeUtils.h>

struct BgfxTextureReadBack {
    u32 ReadyFrame;
    void *Buffer;
};

Unit(BgfxTextureReadBack)
    Component(BgfxTextureReadBack)

#endif //PLAZA_BGFXTEXTUREREADBACK_H
