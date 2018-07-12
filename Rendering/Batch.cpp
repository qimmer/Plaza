//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/String.h>
#include "Batch.h"

struct Batch {
    Batch() : BatchWorldMatrix(m4x4f_Identity), BatchProgram(0), BatchMaterial(0), BatchMesh(0) {}

    Entity BatchMaterial, BatchMesh, BatchProgram;
    v4i BatchScissor;
    m4x4f BatchWorldMatrix;
    Entity BatchShaderVariation;
};

BeginUnit(Batch)
    BeginComponent(Batch)
    RegisterProperty(v4i, BatchScissor))
    RegisterProperty(Entity, BatchMaterial))
    RegisterProperty(Entity, BatchMesh))
    RegisterProperty(Entity, BatchShaderVariation))
    RegisterProperty(m4x4f, BatchWorldMatrix))
EndComponent()

RegisterProperty(v4i, BatchScissor)
RegisterProperty(Entity, BatchMaterial)
RegisterProperty(Entity, BatchMesh)
RegisterProperty(Entity, BatchShaderVariation)
RegisterProperty(m4x4f, BatchWorldMatrix)
