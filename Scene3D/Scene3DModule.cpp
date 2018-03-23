//
// Created by Kim Johannsen on 01/02/2018.
//

#include <Scene/SceneModule.h>
#include "VoxelChunk.h"
#include "VoxelMesh.h"
#include "Scene3DModule.h"
#include "Camera3D.h"
#include "Transform3D.h"
#include "SimpleSceneRenderer.h"
#include "VoxelPalette.h"
#include "VoxelColor.h"
#include "VoxPersistance.h"

DefineModule(Scene3D)
    ModuleDependency(Scene)

    ModuleService(Camera3D)
    ModuleService(Transform3D)
    ModuleService(VoxelPalette)
    ModuleService(VoxelMesh)
    ModuleService(VoxPersistance)

    ModuleType(Camera3D)
    ModuleType(Transform3D)
    ModuleType(VoxelChunk)
    ModuleType(VoxelColor)
    ModuleType(VoxelMesh)
    ModuleType(VoxelPalette)
EndModule()
