//
// Created by Kim Johannsen on 01-04-2018.
//

#include "VoxelChunkSpawner.h"

struct VoxelChunkSpawner {
    Entity VoxelChunkSpawnerCenterEntity, VoxelChunkSpawnerTemplateEntity;
    u8 VoxelChunkSpawnerRadius;
};

BeginUnit(VoxelChunkSpawner)
    BeginComponent(VoxelChunkSpawner)
    RegisterProperty(Entity, VoxelChunkSpawnerCenterEntity))
    RegisterProperty(Entity, VoxelChunkSpawnerTemplateEntity))
    RegisterProperty(u8, VoxelChunkSpawnerRadius))
EndComponent()
EndUnit()
(Entity, VoxelChunkSpawnerCenterEntity)
RegisterProperty(Entity, VoxelChunkSpawnerTemplateEntity)
RegisterProperty(u8, VoxelChunkSpawnerRadius)

DefineService(VoxelChunkSpawner)

EndService()