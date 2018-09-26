//
// Created by Kim on 20-09-2018.
//

#include "SceneRenderer.h"
#include "Frustum.h"
#include "Renderable.h"
#include "Material.h"
#include <Rendering/ShaderCache.h>
#include <Rendering/Uniform.h>
#include <Rendering/CommandList.h>
#include <Rendering/Batch.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/RenderState.h>
#include <Scene/Camera.h>
#include <Scene/Scene.h>

struct RenderPass {
    Entity RenderPassShaderCache;
    Entity RenderPassRenderState;
    float RenderPassClearDepth;
    rgba8 RenderPassClearColor;
    u8 RenderPassClearStencil, RenderPassClearTargets;
    bool RenderPassForceOrder;
};

struct RenderPath {
    Vector(RenderPathPasses, Entity, 8);
};

struct Batch {
    Entity BatchRenderable, BatchBinaryProgram;
    bool BatchDisabled;
};

struct CommandList {
    Vector(CommandListBatches, Entity, 512)
    Entity CommandListPass;
};

struct SceneRenderer {
    Entity SceneRendererScene, SceneRendererTarget, SceneRendererCamera, SceneRendererPath;
    Vector(SceneRendererCommandLists, Entity, 8)
    Vector(SceneRendererUniformStates, Entity, 16)
    v4i SceneRendererViewport;
};

LocalFunction(OnMaterialProgramChanged, void, Entity material, Entity oldProgram, Entity newProgram) {
    for_entity(batch, batchData, Batch) {
        auto renderableMaterial = GetRenderableMaterial(batchData->BatchRenderable);
        if(renderableMaterial == material) {
            auto commandList = GetOwner(batch);
            auto pass = GetCommandListPass(commandList);
            auto shaderCache = GetRenderPassShaderCache(pass);

            SetBatchBinaryProgram(batch, GetShaderCacheBinaryProgram(shaderCache, newProgram));
        }
    }
}

LocalFunction(OnBatchRenderableChanged, void, Entity batch, Entity oldRenderable, Entity newRenderable) {
    auto commandList = GetOwner(batch);
    auto pass = GetCommandListPass(commandList);
    auto shaderCache = GetRenderPassShaderCache(pass);
    auto material = GetRenderableMaterial(newRenderable);

    SetBatchBinaryProgram(batch, GetShaderCacheBinaryProgram(shaderCache, GetMaterialProgram(material)));
}

LocalFunction(OnRenderableMaterialChanged, void, Entity renderable, Entity oldMaterial, Entity newMaterial) {
    for_entity(batch, data, Batch) {
        if(data->BatchRenderable == renderable) {
            OnBatchRenderableChanged(batch, renderable, renderable);
        }
    }
}

BeginUnit(SceneRenderer)
    BeginComponent(RenderPass)
        RegisterChildProperty(ShaderCache, RenderPassShaderCache)
        RegisterChildProperty(RenderState, RenderPassRenderState)
    EndComponent()

    BeginComponent(RenderPath)
        RegisterArrayProperty(RenderPass, RenderPathPasses)
    EndComponent()

    BeginComponent(Batch)
        RegisterProperty(bool, BatchDisabled)
        RegisterReferenceProperty(Renderable, BatchRenderable)
        RegisterReferenceProperty(BinaryProgram, BatchBinaryProgram)
    EndComponent()

    BeginComponent(CommandList)
        RegisterReferenceProperty(RenderPass, CommandListPass)
        RegisterArrayProperty(Batch, CommandListBatches)
    EndComponent()

    BeginComponent(SceneRenderer)
        RegisterReferenceProperty(Scene, SceneRendererScene)
        RegisterReferenceProperty(RenderTarget, SceneRendererTarget)
        RegisterReferenceProperty(Camera, SceneRendererCamera)
        RegisterReferenceProperty(RenderPath, SceneRendererPath)
        RegisterProperty(v4i, SceneRendererViewport)

        RegisterArrayProperty(UniformState, SceneRendererUniformStates)
        RegisterArrayProperty(CommandList, SceneRendererCommandLists)
    EndComponent()

    RegisterSubscription(SceneRendererPathChanged, OnSceneRendererChanged, 0)
    RegisterSubscription(RenderPathPassesChanged, OnRenderPathChanged, 0)
    RegisterSubscription(RenderPathPassesChanged, SceneRendererChanged, 0)
EndUnit()
