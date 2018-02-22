//
// Created by Kim Johannsen on 01/02/2018.
//

#include <Scene/SceneModule.h>
#include "Scene2DModule.h"
#include "Camera2D.h"
#include "SimpleSceneRenderer.h"
#include "Sprite.h"
#include "Transform2D.h"
#include "Sprite.h"


    DefineModule(Scene2D)
        ModuleDependency(Scene)

        ModuleService(Camera2D)
        ModuleService(SimpleSceneRenderer)
        ModuleService(Sprite)
        ModuleService(Transform2D)

        ModuleType(Camera2D)
        ModuleType(SimpleSceneRenderer)
        ModuleType(Sprite)
        ModuleType(Sprite)
        ModuleType(Transform2D)
    EndModule()
