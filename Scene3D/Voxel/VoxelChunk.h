//
// Created by Kim Johannsen on 13-03-2018.
//

#ifndef PLAZA_VOXELCHUNK_H
#define PLAZA_VOXELCHUNK_H

#include <Core/NativeUtils.h>
#include "VoxelPalette.h"

Unit(VoxelChunk)
    Component(VoxelChunk)
        Property(v3i, VoxelChunkSize)

void SetVoxelChunkRegion(Entity voxelChunk, v3i regionOrigin, v3i regionSize, v3i bufferOrigin, v3i bufferSize, const Voxel *source);
void GetVoxelChunkRegion(Entity voxelChunk, v3i regionOrigin, v3i regionSize, v3i bufferOrigin, v3i bufferSize, Voxel *destination);

#endif //PLAZA_VOXELCHUNK_H
