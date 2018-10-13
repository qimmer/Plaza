//
// Created by Kim on 20-09-2018.
//

#include "SceneRenderer.h"
#include "Frustum.h"
#include "Renderable.h"
#include "Material.h"
#include <Rendering/ShaderCache.h>
#include <Rendering/Uniform.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/RenderState.h>
#include <Scene/Camera.h>
#include <Scene/Scene.h>
#include <Foundation/AppNode.h>

struct RenderPass {
    Entity RenderPassShaderCache;
    Entity RenderPassRenderState;
    float RenderPassClearDepth;
    rgba8 RenderPassClearColor;
    u8 RenderPassClearStencil, RenderPassClearTargets;
    bool RenderPassForceOrder;
    Vector(RenderPassSceneUniforms, Entity, 16);
    Vector(RenderPassCameraUniforms, Entity, 16);
    Vector(RenderPassMaterialUniforms, Entity, 32);
    Vector(RenderPassRenderableUniforms, Entity, 16);
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
    v4f SceneRendererViewport;
};

static void SyncBatches(Entity commandList) {
    auto sceneRenderer = GetOwner(commandList);
    auto scene = GetSceneRendererScene(sceneRenderer);

    auto numExisting = GetNumCommandListBatches(commandList);
    auto numBatches = 0;
    {
        for_entity(renderable, data, Renderable) {
            if(GetAppNodeRoot(renderable) != scene) continue;

            numBatches++;
        }
    }

    SetArrayPropertyCount(PropertyOf_CommandListBatches(), commandList, numBatches);

    auto batches = GetCommandListBatches(commandList);
    {
        auto i = 0;
        for_entity(renderable, data, Renderable) {
            if(GetAppNodeRoot(renderable) != scene) continue;

            SetBatchRenderable(batches[i], renderable);

            i++;
        }
    }

}

static void SyncCommandLists(Entity sceneRenderer) {
    auto renderPath = GetSceneRendererPath(sceneRenderer);
    auto renderPathData = GetRenderPathData(renderPath);

    if(!renderPathData) return;

    auto numRenderPasses = renderPathData->RenderPathPasses.Count;

    SetArrayPropertyCount(PropertyOf_SceneRendererCommandLists(), sceneRenderer, numRenderPasses);
    auto commandLists = GetSceneRendererCommandLists(sceneRenderer);
    for(auto i = 0; i < numRenderPasses; ++i) {
        SetCommandListPass(commandLists[i], GetVector(renderPathData->RenderPathPasses)[i]);

        SyncBatches(commandLists[i]);
    }
}

LocalFunction(OnBatchRenderableChanged, void, Entity batch, Entity oldRenderable, Entity newRenderable) {
    auto commandList = GetOwner(batch);
    auto pass = GetCommandListPass(commandList);
    auto shaderCache = GetRenderPassShaderCache(pass);
    auto material = GetRenderableMaterial(newRenderable);

    if(!HasComponent(material, ComponentOf_Material())) return;

    SetBatchBinaryProgram(batch, GetShaderCacheBinaryProgram(shaderCache, GetMaterialProgram(material)));
}

LocalFunction(OnRenderableMaterialChanged, void, Entity renderable, Entity oldMaterial, Entity newMaterial) {
    for_entity(batch, data, Batch) {
        if(data->BatchRenderable == renderable) {
            OnBatchRenderableChanged(batch, renderable, renderable);
        }
    }
}

LocalFunction(OnMaterialProgramChanged, void, Entity material, Entity oldProgram, Entity newProgram) {
    for_entity(renderable, data, Renderable) {
        if(GetRenderableMaterial(renderable) == material) {
            OnRenderableMaterialChanged(renderable, material, material);
        }

    }
}

LocalFunction(OnSceneRendererPathChanged, void, Entity sceneRenderer, Entity oldPath, Entity newPath) {
    SyncCommandLists(sceneRenderer);
}

LocalFunction(OnSceneRendererSceneChanged, void, Entity sceneRenderer, Entity oldScene, Entity newScene) {
    SyncCommandLists(sceneRenderer);
}

LocalFunction(OnRenderPathPassesChanged, void, Entity renderPath, Entity oldPass, Entity newPass) {
    for_entity(sceneRenderer, data, SceneRenderer) {
        if(data->SceneRendererPath == renderPath) {
            SyncCommandLists(sceneRenderer);
        }
    }
}

LocalFunction(OnSceneNodeSceneChanged, void, Entity sceneNode, Entity oldScene, Entity newScene) {
    if(HasComponent(sceneNode, ComponentOf_Renderable())) {
        for_entity(sceneRenderer, data, SceneRenderer) {
            if((oldScene && data->SceneRendererScene == oldScene) || (newScene && data->SceneRendererScene == newScene)) {
                for_children(commandList, SceneRendererCommandLists, sceneRenderer) {
                    SyncBatches(commandList);
                }
            }
        }
    }
}

LocalFunction(OnRenderableAdded, void, Entity component, Entity renderable) {
    OnSceneNodeSceneChanged(renderable, GetAppNodeRoot(renderable), GetAppNodeRoot(renderable));
}

BeginUnit(SceneRenderer)
    BeginEnum(ClearTarget, true)
        RegisterFlag(ClearTarget_None)
        RegisterFlag(ClearTarget_Color)
        RegisterFlag(ClearTarget_Depth)
        RegisterFlag(ClearTarget_Stencil)
    EndEnum()

    BeginComponent(RenderPass)
        RegisterChildProperty(ShaderCache, RenderPassShaderCache)
        RegisterChildProperty(RenderState, RenderPassRenderState)
        RegisterArrayProperty(Uniform, RenderPassSceneUniforms)
        RegisterArrayProperty(Uniform, RenderPassCameraUniforms)
        RegisterArrayProperty(Uniform, RenderPassMaterialUniforms)
        RegisterArrayProperty(Uniform, RenderPassRenderableUniforms)
        RegisterProperty(rgba8, RenderPassClearColor)
        RegisterProperty(float, RenderPassClearDepth)
        RegisterProperty(u8, RenderPassClearStencil)
        RegisterPropertyEnum(u8, RenderPassClearTargets, ClearTarget)
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
        RegisterProperty(v4f, SceneRendererViewport)
        RegisterArrayProperty(CommandList, SceneRendererCommandLists)
    EndComponent()

    RegisterSubscription(SceneRendererPathChanged, OnSceneRendererPathChanged, 0)
    RegisterSubscription(SceneRendererSceneChanged, OnSceneRendererSceneChanged, 0)
    RegisterSubscription(AppNodeRootChanged, OnSceneNodeSceneChanged, 0)
    RegisterSubscription(RenderPathPassesChanged, OnRenderPathPassesChanged, 0)
    RegisterSubscription(BatchRenderableChanged, OnBatchRenderableChanged, 0)
    RegisterSubscription(RenderableMaterialChanged, OnRenderableMaterialChanged, 0)
    RegisterSubscription(MaterialProgramChanged, OnMaterialProgramChanged, 0)
    RegisterSubscription(EntityComponentAdded, OnRenderableAdded, ComponentOf_Renderable())
EndUnit()
