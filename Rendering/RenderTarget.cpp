//
// Created by Kim Johannsen on 14/01/2018.
//

#include "RenderTarget.h"
#include "RenderContext.h"

struct RenderTarget {
    v2i RenderTargetSize;
    Vector(RenderTargetRenderers, Entity, 16)
};

BeginUnit(RenderTarget)
    BeginComponent(RenderTarget)
        RegisterProperty(v2i, RenderTargetSize)
    EndComponent()
EndUnit()