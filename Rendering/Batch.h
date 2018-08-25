//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_COMMANDLISTBATCH_H
#define PLAZA_COMMANDLISTBATCH_H

#include <Core/NativeUtils.h>

Unit(Batch)
    Component(Batch)
        Property(bool, BatchDisabled)
        Property(v4i, BatchScissor)
        ReferenceProperty(Material, BatchMaterial)
        ReferenceProperty(SubMesh, BatchSubMesh)
        ReferenceProperty(BinaryProgram, BatchBinaryProgram)
        Property(m4x4f, BatchWorldMatrix)
        ArrayProperty(UniformState, BatchUniformStates)

#endif //PLAZA_COMMANDLISTBATCH_H
