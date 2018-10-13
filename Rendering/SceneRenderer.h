//
// Created by Kim on 20-09-2018.
//

#ifndef PLAZA_SCENERENDERER_H
#define PLAZA_SCENERENDERER_H

#include <Core/NativeUtils.h>

#define ClearTarget_None 0
#define ClearTarget_Color 1
#define ClearTarget_Depth 2
#define ClearTarget_Stencil 4

Unit(SceneRenderer)
    Enum(ClearTarget)

    Component(RenderPass)
        ChildProperty(ShaderCache, RenderPassShaderCache)
        ChildProperty(RenderState, RenderPassRenderState)
        Property(float, RenderPassClearDepth)
        Property(rgba8, RenderPassClearColor)
        Property(u8, RenderPassClearStencil)
        Property(u8, RenderPassClearTargets)
        Property(bool, RenderPassForceOrder)
        ArrayProperty(Uniform, RenderPassSceneUniforms)
        ArrayProperty(Uniform, RenderPassCameraUniforms)
        ArrayProperty(Uniform, RenderPassMaterialUniforms)
        ArrayProperty(Uniform, RenderPassRenderableUniforms)

    Component(RenderPath)
        ArrayProperty(RenderPathPass, RenderPathPasses)

    Component(Batch)
        Property(bool, BatchDisabled)
        ReferenceProperty(Renderable, BatchRenderable)
        ReferenceProperty(BinaryProgram, BatchBinaryProgram)

    Component(CommandList)
        ReferenceProperty(RenderPass, CommandListPass)
        ArrayProperty(Batch, CommandListBatches)

    Component(SceneRenderer)
        ReferenceProperty(Scene, SceneRendererScene)
        ReferenceProperty(RenderTarget, SceneRendererTarget)
        ReferenceProperty(Camera, SceneRendererCamera)
        ReferenceProperty(RenderPath, SceneRendererPath)
        Property(v4f, SceneRendererViewport)
        ArrayProperty(CommandList, SceneRendererCommandLists)

#endif //PLAZA_SCENERENDERER_H
