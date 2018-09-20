//
// Created by Kim on 23-08-2018.
//

#include "Renderer.h"

struct Renderer {
    Entity RendererSceneRoot;
};

BeginUnit(Renderer)
    BeginEnum(RenderPassSortMode, false)
        RegisterFlag(RenderPassSortMode_None)
        RegisterFlag(RenderPassSortMode_BackToFront)
        RegisterFlag(RenderPassSortMode_FrontToBack)
    EndEnum()

    BeginComponent(Renderer)
        RegisterReferenceProperty(Ownership, RendererSceneRoot)
    EndComponent()
EndUnit()