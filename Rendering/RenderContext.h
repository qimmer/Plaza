//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_RENDERCONTEXT_H
#define PLAZA_RENDERCONTEXT_H

#include <Core/Types.h>
#include <Core/Entity.h>

struct RenderContext {
    StringRef RenderContextTitle;
    bool RenderContextFullscreen, RenderContextVsync, RenderContextShowDebug, RenderContextShowStats;
};

Unit(RenderContext)
    Component(RenderContext)
        Property(StringRef, RenderContextTitle)
        Property(bool, RenderContextFullscreen)
        Property(bool, RenderContextVsync)
        Property(bool, RenderContextShowDebug)
        Property(bool, RenderContextShowStats)

    Declare(AppLoop, ResourcePrePreparation)
    Declare(AppLoop, ResourcePreparation)
    Declare(AppLoop, ResourceSubmission)
    Declare(AppLoop, ResourceDownload)
    Declare(AppLoop, Present)

#endif //PLAZA_RENDERCONTEXT_H
