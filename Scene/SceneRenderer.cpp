//
// Created by Kim on 20-09-2018.
//

#include "SceneRenderer.h"
#include <Rendering/ShaderCache.h>
#include <Rendering/CommandList.h>
#include <Rendering/RenderTarget.h>
#include <Scene/Camera.h>
#include <Scene/Scene.h>

struct RenderPathPass {
    Entity RenderPathPassShaderCache;
};

struct RenderPath {
    Vector(RenderPathPasses, Entity, 8);
};

struct SceneRendererPass {
    Entity SceneRendererPassCommandList;
    Entity SceneRendererPassPathPass;
};

struct SceneRenderer {
    Entity SceneRendererScene, SceneRendererTarget, SceneRendererCamera, SceneRendererPath;
    Vector(SceneRendererPasses, Entity, 8);
};

static void BuildSceneRendererPasses(Entity sceneRenderer) {
    ClearArray(PropertyOf_SceneRendererPasses(), sceneRenderer);

    auto renderPath = GetSceneRendererPath(sceneRenderer);
    auto passes = GetRenderPathPasses(renderPath);
    if(passes) {
        auto numPasses = GetNumRenderPathPasses(renderPath);
        for(auto i = 0; i < numPasses; ++i) {
            auto scenePass = AddSceneRendererPasses(sceneRenderer);
            SetSceneRendererPassPathPass(scenePass, passes[i]);
        }
    }

    SetArrayPropertyCount(PropertyOf_SceneRendererPasses(), sceneRenderer, )
}

static void BuildCommandLists(Entity sceneRendererPass) {

}

BeginUnit(SceneRenderer)
    BeginComponent(RenderPathPass)
        RegisterChildProperty(ShaderCache, RenderPathPassShaderCache)
    EndComponent()

    BeginComponent(RenderPath)
        RegisterArrayProperty(RenderPathPass, RenderPathPasses)
    EndComponent()

    BeginComponent(SceneRendererPass)
        RegisterChildProperty(CommandList, SceneRendererPassCommandList)
        RegisterReferenceProperty(RenderPathPass, SceneRendererPassPathPass)
    EndComponent()

    BeginComponent(SceneRenderer)
        RegisterReferenceProperty(Scene, SceneRendererScene)
        RegisterReferenceProperty(RenderTarget, SceneRendererTarget)
        RegisterReferenceProperty(Camera, SceneRendererCamera)
        RegisterReferenceProperty(RenderPath, SceneRendererPath)

        RegisterArrayPropertyReadOnly(SceneRendererPass, SceneRendererPasses)
    EndComponent()

    RegisterSubscription()
EndUnit()
