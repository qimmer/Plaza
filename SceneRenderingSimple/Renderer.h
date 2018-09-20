//
// Created by Kim on 23-08-2018.
//

#ifndef PLAZA_RENDERER_H
#define PLAZA_RENDERER_H

#include <Core/NativeUtils.h>

enum {
    RenderPassSortMode_None = 0,
    RenderPassSortMode_BackToFront = 1,
    RenderPassSortMode_FrontToBack = 2
};

Unit(Renderer)
    Enum(RenderPassSortMode)
    Enum(RenderPassTargetType)

    Component(RenderPassTarget)
        Property(u8, RenderPassTargetType)

    Component(RenderPass)
        ChildProperty(ShaderCache, RenderPassShaderCache)
        Property(u64, RenderPassDepthTest)
        Property(u64, RenderPassWriteMask)
        Property(u64, RenderPassMultisampleMode)
        Property(u64, RenderPassBlendMode)
        Property(u64, RenderPassSortMode)
        ChildProperty(CommandList, RenderPassCommandList)

    Component(RenderPath)
        Property(u8, RenderPathBufferCount)
        ArrayProperty(RenderPass, RenderPathInstancePasses)
        ArrayProperty(RenderPass, RenderPathPostPasses)

    Component(PostProcessPass)
        ReferenceProperty(Material, PostProcessPassMaterial)

    Component(Renderer)
        ArrayProperty(OffscreenRenderTarget, RendererBuffers)
        ReferenceProperty(RenderPath, RendererRenderPath)
        ArrayProperty(PostProcessPass, Renderer)



#endif //PLAZA_RENDERER_H
