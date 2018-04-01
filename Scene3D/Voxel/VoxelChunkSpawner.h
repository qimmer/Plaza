//
// Created by Kim Johannsen on 01-04-2018.
//

#ifndef PLAZA_VOXELCHUNKSPAWNER_H
#define PLAZA_VOXELCHUNKSPAWNER_H

#include <Core/Entity.h>

DeclareComponent(VoxelChunkSpawner)
DeclareComponentPropertyReactive(VoxelChunkSpawner, Entity, VoxelChunkSpawnerCenterEntity)
DeclareComponentPropertyReactive(VoxelChunkSpawner, Entity, VoxelChunkSpawnerTemplateEntity)
DeclareComponentPropertyReactive(VoxelChunkSpawner, u8, VoxelChunkSpawnerRadius)

DeclareService(VoxelChunkSpawner)

#endif //PLAZA_VOXELCHUNKSPAWNER_H
