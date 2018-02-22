//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Batch.h"


    struct Batch {
        Batch() : BatchWorldMatrix(m4x4f_Identity) {}

        Entity BatchMaterial, BatchMesh;
        v4i BatchScissor;
        m4x4f BatchWorldMatrix;
    };

    DefineComponent(Batch)
    EndComponent()

    DefineComponentProperty(Batch, v4i, BatchScissor)
    DefineComponentProperty(Batch, Entity, BatchMaterial)
    DefineComponentProperty(Batch, Entity, BatchMesh)
    DefineComponentProperty(Batch, m4x4f, BatchWorldMatrix)
