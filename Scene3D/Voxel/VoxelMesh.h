//
// Created by Kim Johannsen on 13-03-2018.
//

#ifndef PLAZA_VOXELMESH_H
#define PLAZA_VOXELMESH_H

#include <Core/NativeUtils.h>

Unit(VoxelMesh)
    Component(VoxelMesh)

        Property(Entity, VoxelMeshChunk)
        Property(Entity, VoxelMeshPalette)

Entity GetVoxelVertexShader();
Entity GetVoxelPixelShader();
Entity GetVoxelPaletteTextureUniform();
Entity GetVoxelDataRoot();

#endif //PLAZA_VOXELMESH_H
