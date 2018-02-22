//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_COMMANDLISTBATCH_H
#define PLAZA_COMMANDLISTBATCH_H

#include <Core/Entity.h>


    DeclareComponent(Batch)

    DeclareComponentProperty(Batch, v4i, BatchScissor)
    DeclareComponentProperty(Batch, Entity, BatchMaterial)
	DeclareComponentProperty(Batch, Entity, BatchMesh)
    DeclareComponentProperty(Batch, m4x4f, BatchWorldMatrix)

#endif //PLAZA_COMMANDLISTBATCH_H
