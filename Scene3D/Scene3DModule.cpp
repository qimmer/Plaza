//
// Created by Kim Johannsen on 01/02/2018.
//

#include <Scene/SceneModule.h>
#include "Scene3DModule.h"
#include "Camera3D.h"
#include "Transform3D.h"
#include "SimpleSceneRenderer.h"

    DefineModule(Scene3D)
        ModuleDependency(Scene)

        ModuleService(Camera3D)
        ModuleService(Transform3D)

        ModuleType(Camera3D)
        ModuleType(Transform3D)
    EndModule()
