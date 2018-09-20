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
        ReferenceProperty(Renderable, BatchRenderable)
        ReferenceProperty(BinaryProgram, BatchBinaryProgram)

#endif //PLAZA_COMMANDLISTBATCH_H
