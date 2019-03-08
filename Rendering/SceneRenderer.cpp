//
// Created by Kim on 20-09-2018.
//

#include "SceneRenderer.h"
#include "Frustum.h"
#include "Renderable.h"
#include "Material.h"
#include "Scene.h"
#include "RenderingModule.h"
#include <Rendering/ShaderCache.h>
#include <Rendering/Uniform.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/RenderState.h>
#include <Scene/Camera.h>
#include <Scene/Scene.h>
#include <Foundation/AppLoop.h>

static void SyncBatches(Entity commandList) {
    auto sceneRenderer = GetOwner(commandList);
    auto scene = GetSceneRendererScene(sceneRenderer);

    u32 numExisting = GetCommandListBatches(commandList).size();
    
    u32 numBatches = 0;
    {
        for_entity(renderable, data, Renderable) {
            if(GetSceneNodeScene(renderable) != scene) continue;

            numBatches++;
        }
    }

    SetArrayPropertyCount(PropertyOf_CommandListBatches(), commandList, numBatches);

    auto& batches = GetCommandListBatches(commandList);
    {
        auto i = 0;
        for_entity(renderable, data, Renderable) {
            if(GetSceneNodeScene(renderable) != scene) continue;

            SetBatchRenderable(batches[i], renderable);

            i++;
        }
    }

}

static void SyncCommandLists(Entity sceneRenderer) {
    auto renderPath = GetSceneRendererPath(sceneRenderer);
    auto renderPathData = GetRenderPathData(renderPath);

    if(!renderPathData) return;

    auto& renderPasses = GetRenderPathPasses(renderPath);

    SetArrayPropertyCount(PropertyOf_SceneRendererCommandLists(), sceneRenderer, renderPasses.size());
    auto& commandLists = GetSceneRendererCommandLists(sceneRenderer);
    for(auto i = 0; i < renderPasses.size(); ++i) {
        SetCommandListPass(commandLists[i], renderPasses[i]);

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

LocalFunction(OnSyncCommandLists, void) {
	for_entity(sceneRenderer, data, SceneRenderer) {
		for_children(commandList, SceneRendererCommandLists, sceneRenderer) {
			SyncBatches(commandList);
		}
	}
}

BeginUnit(SceneRenderer)
    BeginEnum(ClearTarget, true)
        RegisterFlag(ClearTarget_None)
        RegisterFlag(ClearTarget_Color)
        RegisterFlag(ClearTarget_Depth)
        RegisterFlag(ClearTarget_Stencil)
    EndEnum()

    BeginEnum(RenderingSortMode, false)
        RegisterFlag(RenderingSortMode_Default)
        RegisterFlag(RenderingSortMode_Sequental)
        RegisterFlag(RenderingSortMode_DepthAscending)
        RegisterFlag(RenderingSortMode_DepthDescending)
    EndEnum()

    BeginComponent(RenderPass)
        RegisterChildProperty(ShaderCache, RenderPassShaderCache)
        RegisterChildProperty(RenderState, RenderPassRenderState)
        RegisterArrayProperty(Uniform, RenderPassSceneUniforms)
        RegisterArrayProperty(Uniform, RenderPassCameraUniforms)
        RegisterArrayProperty(Uniform, RenderPassRenderableUniforms)
        RegisterArrayProperty(Uniform, RenderPassMaterialUniforms)
        RegisterArrayProperty(Uniform, RenderPassMeshUniforms)
        RegisterProperty(rgba8, RenderPassClearColor)
        RegisterProperty(float, RenderPassClearDepth)
        RegisterProperty(u8, RenderPassClearStencil)
        RegisterPropertyEnum(u8, RenderPassClearTargets, ClearTarget)
        RegisterPropertyEnum(u8, RenderPassSortMode, RenderingSortMode)
    EndComponent()

    BeginComponent(RenderPath)
        RegisterArrayProperty(RenderPass, RenderPathPasses)
    EndComponent()

    BeginComponent(Batch)
        RegisterPropertyReadOnly(bool, BatchDisabled)
        RegisterReferencePropertyReadOnly(Renderable, BatchRenderable)
        RegisterReferencePropertyReadOnly(BinaryProgram, BatchBinaryProgram)
    EndComponent()

    BeginComponent(CommandList)
        RegisterReferencePropertyReadOnly(RenderPass, CommandListPass)
        RegisterArrayPropertyReadOnly(Batch, CommandListBatches)
    EndComponent()

    BeginComponent(SceneRenderer)
        RegisterReferenceProperty(Scene, SceneRendererScene)
        RegisterReferenceProperty(RenderTarget, SceneRendererTarget)
        RegisterReferenceProperty(Camera, SceneRendererCamera)
        RegisterReferenceProperty(RenderPath, SceneRendererPath)
        RegisterProperty(v4f, SceneRendererViewport)
        RegisterArrayPropertyReadOnly(CommandList, SceneRendererCommandLists)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_SceneRendererPath()), OnSceneRendererPathChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_SceneRendererScene()), OnSceneRendererSceneChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderPathPasses()), OnRenderPathPassesChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BatchRenderable()), OnBatchRenderableChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderableMaterial()), OnRenderableMaterialChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MaterialProgram()), OnMaterialProgramChanged, 0)

    SetAppLoopOrder(AppLoopOf_BatchSubmission(), AppLoopOrder_BatchSubmission);

	RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnSyncCommandLists, AppLoopOf_BatchSubmission())
EndUnit()
