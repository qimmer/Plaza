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

template<typename T, size_t fixedCapacity = 128> using Vector = std::vector<T>;
template<typename T> using Set = eastl::unordered_set<T, std::hash<T>, std::equal_to<T>>;
template<typename T, typename Comparator = std::less<T>> using OrderedSet = eastl::set<T, Comparator>;

#define Vector(NAME, TYPE, SMALLCAP) \
    static const u32 StaticCap ## NAME = SMALLCAP;\
    static const u32 ElementSize_ ## NAME = sizeof(TYPE);\
    u32 Num ## NAME; \
    u32 Cap ## NAME; \
    TYPE * NAME; \
    TYPE Buf ## NAME [SMALLCAP];

#define SetVectorAmount(DATA, NAME, NEWCOUNT) __SetVectorAmount(& DATA -> Num ## NAME, (NEWCOUNT), & DATA -> Cap ## NAME, DATA -> StaticCap ## NAME, (void**)& DATA -> NAME, &DATA -> Buf ## NAME[0], DATA->ElementSize_ ## NAME )
#define VectorRemove(DATA, NAME, INDEX) DATA -> NAME [INDEX] = DATA -> NAME [DATA -> Num ## NAME - 1]; SetVectorAmount(DATA, NAME, DATA -> Num ## NAME - 1)
#define VectorAdd(DATA, NAME, VALUE) SetVectorAmount(DATA, NAME, DATA -> Num ## NAME + 1); DATA -> NAME [DATA -> Num ## NAME - 1] = VALUE
#define VectorClear(DATA, NAME) SetVectorAmount(DATA, NAME, 0)

void __SetVectorAmount(unsigned int* num, unsigned int newNum, unsigned int* dynCap, unsigned int staCap, void **curData, void *staBuf, unsigned int elementSize);

#endif //PLAZA_VECTOR_H
