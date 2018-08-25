//
// Created by Kim on 23-08-2018.
//

#include "Renderer.h"

struct Renderer {
    Entity RendererSceneRoot;
};

BeginUnit(Renderer)
    BeginComponent(Renderer)
        RegisterReferenceProperty(Ownership, RendererSceneRoot)
    EndComponent()
EndUnit()