//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Batch.h"

struct Batch {
    Batch() : BatchWorldMatrix(m4x4f_Identity), BatchProgram(0), BatchMaterial(0), BatchMesh(0) {}

    Entity BatchMaterial, BatchMesh, BatchProgram;
    v4i BatchScissor;
    m4x4f BatchWorldMatrix;
};

DefineComponent(Batch)
    DefineProperty(v4i, BatchScissor)
    DefineProperty(Entity, BatchMaterial)
    DefineProperty(Entity, BatchMesh)
    DefineProperty(Entity, BatchProgram)
    DefineProperty(m4x4f, BatchWorldMatrix)
EndComponent()

DefineComponentProperty(Batch, v4i, BatchScissor)
DefineComponentProperty(Batch, Entity, BatchMaterial)
DefineComponentProperty(Batch, Entity, BatchMesh)
DefineComponentProperty(Batch, Entity, BatchProgram)
DefineComponentProperty(Batch, m4x4f, BatchWorldMatrix)
