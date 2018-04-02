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

template<typename T, size_t fixedCapacity = 128> using Vector = eastl::vector<T>;
template<typename T> using Set = eastl::unordered_set<T, std::hash<T>, std::equal_to<T>>;
template<typename T, typename Comparator = std::less<T>> using OrderedSet = eastl::set<T, Comparator>;

#endif //PLAZA_VECTOR_H
