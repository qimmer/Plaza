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
    auto sceneRenderer = GetOwnership(commandList).Owner;
    auto scene = GetSceneRenderer(sceneRenderer).SceneRendererScene;
    auto pass = GetCommandList(commandList).CommandListPass;
    auto shaderCache = GetRenderPass(pass).RenderPassShaderCache;
    auto commandListData = GetCommandList(commandList);

    u32 numBatches = 0;
    for_entity(renderable, ComponentOf_Renderable()) {
        if(GetSceneNode(renderable).SceneNodeScene != scene) continue;

        numBatches++;
    }

    commandListData.CommandListBatches.SetSize(numBatches);

    auto i = 0;
    Renderable renderableData;
    for_entity_data(renderable, ComponentOf_Renderable(), &renderableData) {
        if(GetSceneNode(renderable).SceneNodeScene != scene) continue;

        if(!commandListData.CommandListBatches[i]) {
            commandListData.CommandListBatches[i] = CreateEntity();
        }

        auto batchData = GetBatch(commandListData.CommandListBatches[i]);
        batchData.BatchRenderable = renderable;
        batchData.BatchBinaryProgram = GetShaderCacheBinaryProgram(shaderCache, GetMaterial(renderableData.RenderableMaterial).MaterialProgram);
        SetBatch(commandListData.CommandListBatches[i], batchData);

        i++;
    }
}

static void SyncCommandLists(Entity sceneRenderer) {
    auto sceneRendererData = GetSceneRenderer(sceneRenderer);
    auto pathData = GetRenderPath(sceneRendererData.SceneRendererPath);

    sceneRendererData.SceneRendererCommandLists.SetSize(pathData.RenderPathPasses.GetSize());

    for(auto i = 0; i < pathData.RenderPathPasses.GetSize(); ++i) {
        if(!sceneRendererData.SceneRendererCommandLists[i]) {
            sceneRendererData.SceneRendererCommandLists[i] = CreateEntity();
        }
        auto commandListData = GetCommandList(sceneRendererData.SceneRendererCommandLists[i]);
        commandListData.CommandListPass = pathData.RenderPathPasses[i];
        SetCommandList(sceneRendererData.SceneRendererCommandLists[i], commandListData);

        SyncBatches(sceneRendererData.SceneRendererCommandLists[i]);
    }

    SetSceneRenderer(sceneRenderer, sceneRendererData);
}

static void OnRenderableChanged(Entity renderable, const Renderable& oldData, const Renderable& newData) {
    if(oldData.RenderableMaterial != newData.RenderableMaterial) {
        Batch batchData;
        for_entity_data(batch, ComponentOf_Batch(), &batchData) {
            if(batchData.BatchRenderable == renderable) {
                auto commandList = GetOwnership(batch).Owner;
                auto pass = GetCommandList(commandList).CommandListPass;
                auto shaderCache = GetRenderPass(pass).RenderPassShaderCache;

                batchData.BatchBinaryProgram = GetShaderCacheBinaryProgram(shaderCache, GetMaterial(newData.RenderableMaterial).MaterialProgram);
                SetBatch(batch, batchData);
            }
        }
    }
}

static void OnMaterialChanged(Entity material, const Material& oldData, const Material& newData) {
    if(oldData.MaterialProgram != newData.MaterialProgram) {
        Batch batchData;
        for_entity_data(batch, ComponentOf_Batch(), &batchData) {
            auto renderableData = GetRenderable(batchData.BatchRenderable);
            if(renderableData.RenderableMaterial == material) {
                auto commandList = GetOwnership(batch).Owner;
                auto pass = GetCommandList(commandList).CommandListPass;
                auto shaderCache = GetRenderPass(pass).RenderPassShaderCache;

                batchData.BatchBinaryProgram = GetShaderCacheBinaryProgram(shaderCache, newData.MaterialProgram);
                SetBatch(batch, batchData);
            }
        }
    }
}

static void OnSceneRendererChanged(Entity sceneRenderer, Entity oldScene, Entity newScene) {
    SyncCommandLists(sceneRenderer);
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    SceneRenderer sceneRendererData;
	for_entity_data(sceneRenderer, ComponentOf_SceneRenderer(), &sceneRendererData) {
		for(auto commandList : sceneRendererData.SceneRendererCommandLists) {
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
        BeginChildProperty(RenderPassShaderCache)
        EndChildProperty()
        BeginChildProperty(RenderPassRenderState)
        EndChildProperty()
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

    RegisterSystem(OnRenderableChanged, ComponentOf_Renderable())
    RegisterSystem(OnMaterialChanged, ComponentOf_Material())
    RegisterSystem(OnSceneRendererChanged, ComponentOf_SceneRenderer())
    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_Rendering - 0.1f)
EndUnit()
