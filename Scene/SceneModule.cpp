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


    DefineModule(Scene)
        ModuleDependency(Rendering)

        ModuleService(SceneNode)
        ModuleService(TransformUpdateService)

        ModuleType(Camera)
        ModuleType(MeshInstance)
        ModuleType(Scene)
        ModuleType(SceneNode)
        ModuleType(Transform)
    EndModule()
