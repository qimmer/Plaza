//
// Created by Kim Johannsen on 19-03-2018.
//

#ifndef PLAZA_OFFSCREENRENDERTARGET_H
#define PLAZA_OFFSCREENRENDERTARGET_H

#include <Core/NativeUtils.h>

struct OffscreenRenderTarget {
    ChildArray OffscreenRenderTargetTextures;
};

Unit(OffscreenRenderTarget)
    Component(OffscreenRenderTarget)
        ArrayProperty(Texture2D, OffscreenRenderTargetTextures)

#endif //PLAZA_OFFSCREENRENDERTARGET_H
