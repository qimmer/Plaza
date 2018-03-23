//
// Created by Kim Johannsen on 13-03-2018.
//

#ifndef PLAZA_VOXELMESH_H
#define PLAZA_VOXELMESH_H

#include <Core/Entity.h>

DeclareComponent(VoxelMesh)
DeclareService(VoxelMesh)

DeclareComponentPropertyReactive(VoxelMesh, Entity, VoxelMeshChunk)
DeclareComponentPropertyReactive(VoxelMesh, Entity, VoxelMeshPalette)

Entity GetVoxelProgram();
Entity GetVoxelPaletteTextureUniform();
Entity GetVoxelDataRoot();

#endif //PLAZA_VOXELMESH_H
