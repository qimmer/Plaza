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


BeginUnit(RenderContext)
    BeginComponent(RenderContext)
        RegisterBase(RenderTarget)
        RegisterProperty(StringRef, RenderContextTitle)
        RegisterProperty(bool, RenderContextFullscreen)
        RegisterProperty(bool, RenderContextVsync)
        RegisterProperty(bool, RenderContextShowDebug)
        RegisterProperty(bool, RenderContextShowStats)
        RegisterChildProperty(AppLoop, RenderContextLoop)
    EndComponent()
EndUnit()
