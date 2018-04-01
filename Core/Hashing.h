//
// Created by Kim Johannsen on 30-03-2018.
//

#ifndef PLAZA_HASHING_H
#define PLAZA_HASHING_H

#include <cstddef>
#include "Types.h"

inline u32 HashCombine(u32 currentHash, u32 newHashedValue)
{
    return currentHash ^ (newHashedValue + 0x9e3779b9 + (currentHash<<6) + (currentHash>>2));
}

u32 HashCreate(const void* data, u32 size, u32 seed);

#endif //PLAZA_HASHING_H
