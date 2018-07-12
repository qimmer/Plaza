//
// Created by Kim Johannsen on 01-04-2018.
//

#ifndef PLAZA_VOXELCHUNKSPAWNER_H
#define PLAZA_VOXELCHUNKSPAWNER_H

#include <Core/NativeUtils.h>

Unit(VoxelChunkSpawner)
    Component(VoxelChunkSpawner)
        Property(Entity, VoxelChunkSpawnerCenterEntity)
        Property(Entity, VoxelChunkSpawnerTemplateEntity)
        Property(u8, VoxelChunkSpawnerRadius)


#endif //PLAZA_VOXELCHUNKSPAWNER_H
