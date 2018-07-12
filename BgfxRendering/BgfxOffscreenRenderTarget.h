//
// Created by Kim Johannsen on 19-03-2018.
//

#ifndef PLAZA_BGFXOFFSCREENRENDERTARGET_H
#define PLAZA_BGFXOFFSCREENRENDERTARGET_H

#include <Core/NativeUtils.h>

Unit(BgfxOffscreenRenderTarget)
    Component(BgfxOffscreenRenderTarget)

u16 GetBgfxOffscreenRenderTargetHandle(Entity entity);

void UpdatOffscreenRenderTarget2D(Entity entity);

#endif //PLAZA_BGFXOFFSCREENRENDERTARGET_H
