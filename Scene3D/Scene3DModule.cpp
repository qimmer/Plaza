//
// Created by Kim Johannsen on 01/02/2018.
//

#include <Scene/SceneModule.h>
#include "Scene2DModule.h"
#include "Camera2D.h"
#include "SimpleSceneRenderer.h"
#include "Tilemap.h"
#include "Transform2D.h"
#include "Sprite.h"


    DefineModule(Scene3D)
        ModuleDependency(Scene)

        ModuleService(Camera3D)
        ModuleService(Transform3D)

        ModuleType(Camera3D)
        ModuleType(Transform3D)
    EndModule()
}