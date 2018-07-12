//
// Created by Kim Johannsen on 13-03-2018.
//

#include <Foundation/Stream.h>
#include "VoxelChunk.h"

struct VoxelChunk {
    Entity VoxelChunkNeighbour[27];
    v3i VoxelChunkSize;
};

BeginUnit(VoxelChunk)
    BeginComponent(VoxelChunk)
    RegisterBase(Stream)
EndComponent()

RegisterProperty(v3i, VoxelChunkSize)

#define GetStrideZ(SIZE, Z) ((SIZE).x * (SIZE).y * (Z))
#define GetStrideY(SIZE, Y) ((SIZE).x * (Y))
#define GetOffset(STRIDEZ, STRIDEY, X) ((STRIDEZ) + (STRIDEY) + (X))

void SetVoxelChunkRegion(Entity voxelChunk, v3i regionOrigin, v3i regionSize, v3i bufferOrigin, v3i bufferSize, const Voxel *source) {
    v3i regionEnd = {regionOrigin.x + regionSize.x, regionOrigin.y + regionSize.y, regionOrigin.z + regionSize.z};

    auto chunkSize = GetVoxelChunkSize(voxelChunk);

    Assert(regionOrigin.x >= 0 && regionOrigin.y >= 0 && regionOrigin.z >= 0 && regionEnd.x <= chunkSize.x && regionEnd.y <= chunkSize.y && regionEnd.z <= chunkSize.z);

    Assert(StreamOpen(voxelChunk, StreamMode_Write));

    v3i regionCurrent, bufferCurrent;
    for(regionCurrent.z = regionOrigin.z; regionCurrent.z < regionEnd.z; ++regionCurrent.z) {
        bufferCurrent.z = regionCurrent.z - regionOrigin.z + bufferOrigin.z;

        auto chunkStrideZ = GetStrideZ(chunkSize, regionCurrent.z);
        auto bufferStrideZ = GetStrideZ(bufferSize, bufferCurrent.z);

        for(regionCurrent.y = regionOrigin.y; regionCurrent.y < regionEnd.y; ++regionCurrent.y) {
            bufferCurrent.y = regionCurrent.y - regionOrigin.y + bufferOrigin.y;

            auto chunkStrideY = GetStrideY(chunkSize, regionCurrent.y);
            auto bufferStrideY = GetStrideY(bufferSize, bufferCurrent.y);

            auto chunkByteOffset = GetOffset(chunkStrideZ, chunkStrideY, regionOrigin.x) * sizeof(Voxel);
            auto bufferByteOffset = GetOffset(bufferStrideZ, bufferStrideY, bufferOrigin.x) * sizeof(Voxel);

            StreamSeek(voxelChunk, chunkByteOffset);
            Assert(StreamWrite(voxelChunk, sizeof(Voxel) * bufferSize.x, ((const char*)source) + bufferByteOffset) == sizeof(Voxel) * bufferSize.x);
        }
    }
    StreamClose(voxelChunk);
}

#define DivideCoord(C, S) ((C) < 0 ? (-1 + (C) / (S)) : ((C) / (S)))

void GetVoxelChunkRegion(Entity voxelChunk, v3i regionOrigin, v3i regionSize, v3i bufferOrigin, v3i bufferSize, Voxel *destination) {
    v3i regionEnd = {regionOrigin.x + regionSize.x, regionOrigin.y + regionSize.y, regionOrigin.z + regionSize.z};

    auto chunkSize = GetVoxelChunkSize(voxelChunk);

    Assert(regionOrigin.x >= 0 && regionOrigin.y >= 0 && regionOrigin.z >= 0 && regionEnd.x <= chunkSize.x && regionEnd.y <= chunkSize.y && regionEnd.z <= chunkSize.z);

    Assert(StreamOpen(voxelChunk, StreamMode_Read));

    v3i regionCurrent = regionOrigin, bufferCurrent = bufferOrigin;
    for(regionCurrent.z = regionOrigin.z; regionCurrent.z < regionEnd.z; ++regionCurrent.z) {
        bufferCurrent.z = regionCurrent.z - regionOrigin.z + bufferOrigin.z;

        auto chunkStrideZ = GetStrideZ(chunkSize, regionCurrent.z);
        auto bufferStrideZ = GetStrideZ(bufferSize, bufferCurrent.z);

        for(regionCurrent.y = regionOrigin.y; regionCurrent.y < regionEnd.y; ++regionCurrent.y) {
            bufferCurrent.y = regionCurrent.y - regionOrigin.y + bufferOrigin.y;

            auto chunkStrideY = GetStrideY(chunkSize, regionCurrent.y);
            auto bufferStrideY = GetStrideY(bufferSize, bufferCurrent.y);

            auto chunkByteOffset = GetOffset(chunkStrideZ, chunkStrideY, regionOrigin.x) * sizeof(Voxel);
            auto bufferByteOffset = GetOffset(bufferStrideZ, bufferStrideY, bufferOrigin.x) * sizeof(Voxel);

            StreamSeek(voxelChunk, chunkByteOffset);
            StreamRead(voxelChunk, sizeof(Voxel) * regionSize.x, ((char*)destination) + bufferByteOffset);
        }
    }
    StreamClose(voxelChunk);
}
