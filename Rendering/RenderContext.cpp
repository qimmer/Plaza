//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/Debug.h>
#include <Foundation/AppLoop.h>
#include <Input/Key.h>

#include "RenderContext.h"
#include "RenderTarget.h"

#include <cfloat>
#include <Algorithms.h>


struct RenderContext {
    char RenderContextTitle[128];
    bool RenderContextFullscreen, RenderContextVsync;
    Entity RenderContextLoop;
};

BeginUnit(RenderContext)
    BeginComponent(RenderContext)
        RegisterBase(RenderTarget)
        RegisterProperty(StringRef, RenderContextTitle)
        RegisterProperty(bool, RenderContextFullscreen)
        RegisterProperty(bool, RenderContextVsync)
        RegisterChildProperty(AppLoop, RenderContextLoop)
    EndComponent()
EndUnit()
