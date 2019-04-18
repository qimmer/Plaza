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

enum {
    RenderingSortMode_Default,
    RenderingSortMode_Sequental,
    RenderingSortMode_DepthAscending,
    RenderingSortMode_DepthDescending
};

struct RenderPass {
    ChildArray RenderPassSceneUniforms, RenderPassCameraUniforms, RenderPassRenderableUniforms, RenderPassMaterialUniforms, RenderPassMeshUniforms;
    Entity RenderPassShaderCache;
    Entity RenderPassRenderState;
    float RenderPassClearDepth;
    rgba8 RenderPassClearColor;
    u8 RenderPassClearStencil, RenderPassClearTargets, RenderPassSortMode;
    bool RenderPassForceOrder;
};

struct RenderPath {
    ChildArray RenderPathPasses;
};

struct Batch {
    Entity BatchRenderable, BatchBinaryProgram;
    bool BatchDisabled;
};

struct CommandList {
    ChildArray CommandListBatches;
    Entity CommandListPass;
};

struct SceneRenderer {
    ChildArray SceneRendererCommandLists;
    Entity SceneRendererScene, SceneRendererTarget, SceneRendererCamera, SceneRendererPath;
    v4f SceneRendererViewport;
};

Unit(SceneRenderer)
    Enum(ClearTarget)
    Enum(RenderingSortMode)

    Component(RenderPass)
        ChildProperty(ShaderCache, RenderPassShaderCache)
        ChildProperty(RenderState, RenderPassRenderState)
        Property(float, RenderPassClearDepth)
        Property(rgba8, RenderPassClearColor)
        Property(u8, RenderPassClearStencil)
        Property(u8, RenderPassClearTargets)
        Property(u8, RenderPassSortMode)
        Property(bool, RenderPassForceOrder)
        ArrayProperty(Uniform, RenderPassSceneUniforms)
        ArrayProperty(Uniform, RenderPassCameraUniforms)
        ArrayProperty(Uniform, RenderPassRenderableUniforms)
        ArrayProperty(Uniform, RenderPassMaterialUniforms)
        ArrayProperty(Uniform, RenderPassMeshUniforms)

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

    Declare(AppLoop, BatchSubmission)
#endif //PLAZA_SCENERENDERER_H
