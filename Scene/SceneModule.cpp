//
// Created by Kim Johannsen on 01/02/2018.
//

#include <Rendering/RenderingModule.h>
#include "SceneModule.h"
#include "Camera.h"
#include "MeshInstance.h"
#include "SceneNode.h"
#include "Scene.h"
#include "Transform.h"
#include "SimpleSceneRenderer.h"
#include "ScenePicker.h"
#include "MousePicker.h"


DefineModule(Scene)
        ModuleDependency(Rendering)

        ModuleService(SceneNode)
        ModuleService(SimpleSceneRenderer)
        ModuleService(TransformUpdateService)
        ModuleService(Camera)
        ModuleService(ScenePicker)
        ModuleService(MousePicker)

        ModuleType(Camera)
        ModuleType(MeshInstance)
        ModuleType(Scene)
        ModuleType(SceneNode)
        ModuleType(Transform)
        ModuleType(SimpleSceneRenderer)
        ModuleType(ScenePicker)
        ModuleType(MousePicker)
    EndModule()
