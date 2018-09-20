//
// Created by Kim on 20-09-2018.
//

#ifndef PLAZA_SCENERENDERER_H
#define PLAZA_SCENERENDERER_H

#include <Core/NativeUtils.h>

Unit(SceneRenderer)

    Component(RenderPathPass)
        ChildProperty(ShaderCache, RenderPathPassShaderCache)

    Component(RenderPath)
        ArrayProperty(RenderPathPass, RenderPathPasses)

    Component(SceneRendererPass)
        ChildProperty(CommandList, SceneRendererPassCommandList)
        ReferenceProperty(RenderPathPass, SceneRendererPassPathPass)

    Component(SceneRenderer)
        ReferenceProperty(Scene, SceneRendererScene)
        ReferenceProperty(RenderTarget, SceneRendererTarget)
        ReferenceProperty(Camera, SceneRendererCamera)
        ReferenceProperty(RenderPath, SceneRendererPath)

        ArrayProperty(SceneRendererPass, SceneRendererPasses)

#endif //PLAZA_SCENERENDERER_H
