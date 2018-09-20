//
// Created by Kim Johannsen on 01/02/2018.
//

#include <Scene/SceneModule.h>
#include "Scene3D/Voxel/VoxelChunk.h"
#include "Scene3D/Voxel/VoxelMesh.h"
#include "Scene3DModule.h"
#include "Camera3D.h"
#include "Transform3D.h"
#include "SceneRenderingSimple/SimpleSceneRenderer.h"
#include "Scene3D/Voxel/VoxelPalette.h"
#include "Scene3D/Voxel/VoxelColor.h"
#include "Scene3D/Voxel/VoxPersistance.h"

BeginModule(Scene3D)
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
