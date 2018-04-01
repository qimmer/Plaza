//
// Created by Kim Johannsen on 01-04-2018.
//

#include "VoxelChunkSpawner.h"

struct VoxelChunkSpawner {
    Entity VoxelChunkSpawnerCenterEntity, VoxelChunkSpawnerTemplateEntity;
    u8 VoxelChunkSpawnerRadius;
};

DefineComponent(VoxelChunkSpawner)
    DefineProperty(Entity, VoxelChunkSpawnerCenterEntity)
    DefineProperty(Entity, VoxelChunkSpawnerTemplateEntity)
    DefineProperty(u8, VoxelChunkSpawnerRadius)
EndComponent()

DefineComponentPropertyReactive(VoxelChunkSpawner, Entity, VoxelChunkSpawnerCenterEntity)
DefineComponentPropertyReactive(VoxelChunkSpawner, Entity, VoxelChunkSpawnerTemplateEntity)
DefineComponentPropertyReactive(VoxelChunkSpawner, u8, VoxelChunkSpawnerRadius)

DefineService(VoxelChunkSpawner)

EndService()