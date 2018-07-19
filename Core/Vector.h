//
// Created by Kim Johannsen on 03/01/2018.
//

#ifndef PLAZA_VECTOR_H
#define PLAZA_VECTOR_H

#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/unordered_set.h>
#include <EASTL/set.h>

#include <Core/StackContainer.h>
#include <Core/AlignedAllocator.h>
#include "Types.h"

template<typename T, size_t fixedCapacity = 128> using Vector = std::vector<T>;
template<typename T> using Set = eastl::unordered_set<T, std::hash<T>, std::equal_to<T>>;
template<typename T, typename Comparator = std::less<T>> using OrderedSet = eastl::set<T, Comparator>;

#define Vector(NAME, TYPE, SMALLCAP) \
    struct {\
        u32 Count; \
        u32 DynCapacity; \
        TYPE * DynBuf; \
        TYPE StaBuf [SMALLCAP];\
    } NAME;

struct VectorStruct {
    u32 Count;
    u32 DynCapacity;
    char *DynBuf;
    char StaBuf[1];
};

#define GetVector(VSTRUCT) ((VSTRUCT).DynBuf ? (VSTRUCT).DynBuf : (VSTRUCT).StaBuf)
#define SetVectorAmount(VSTRUCT, NEWCOUNT) __SetVectorAmount(& VSTRUCT.Count, (NEWCOUNT), & VSTRUCT.DynCapacity, sizeof(VSTRUCT.StaBuf)/sizeof(VSTRUCT.StaBuf[0]), (void**)& VSTRUCT.DynBuf, &VSTRUCT.StaBuf[0], sizeof(VSTRUCT.StaBuf[0]) )
#define VectorRemove(VSTRUCT, INDEX) GetVector(VSTRUCT)[INDEX] = GetVector(VSTRUCT)[VSTRUCT.Count - 1]; SetVectorAmount(VSTRUCT, VSTRUCT.Count - 1)
#define VectorAdd(VSTRUCT, VALUE) SetVectorAmount(VSTRUCT, VSTRUCT.Count + 1); GetVector(VSTRUCT)[VSTRUCT.Count - 1] = VALUE
#define VectorClear(VSTRUCT) SetVectorAmount(VSTRUCT, 0)

void __SetVectorAmount(unsigned int* num, unsigned int newNum, unsigned int* dynCap, unsigned int staCap, void **dynBuf, void *staBuf, unsigned int elementSize);

#endif //PLAZA_VECTOR_H
