//
// Created by Kim Johannsen on 07-03-2018.
//

#ifndef PLAZA_PERFRAMESTACKALLOCATOR_H
#define PLAZA_PERFRAMESTACKALLOCATOR_H

#include "Types.h"

void PerFrameStackReserve(u32 size);
void PerFrameStackReset();
void* PerFrameStackAlloc(u32 size);

#endif //PLAZA_PERFRAMESTACKALLOCATOR_H
