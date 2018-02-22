
#ifndef VoxelChunk_H
#define VoxelChunk_H

#include <Core/Entity.h>


    DeclareComponent(VoxelChunk)
    DeclareService(VoxelChunk)

    DeclareComponentProperty(VoxelChunk, Entity, VoxelChunkStream)
    DeclareComponentProperty(VoxelChunk, v3i, VoxelChunkOffset)

#endif //VoxelChunk_H
