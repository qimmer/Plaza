//
// Created by Kim Johannsen on 14/01/2018.
//

#include "RenderTarget.h"
#include "Context.h"

struct RenderTarget {
    RenderTarget() : RenderTargetSize({256, 256}) {}

    v2i RenderTargetSize;
};

DefineComponent(RenderTarget)
    DefinePropertyReactive(v2i, RenderTargetSize)
EndComponent()

DefineComponentPropertyReactive(RenderTarget, v2i, RenderTargetSize)
