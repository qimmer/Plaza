//
// Created by Kim Johannsen on 03/01/2018.
//

#ifndef PLAZA_VECTOR_H
#define PLAZA_VECTOR_H

#include <vector>
#include <Core/StackContainer.h>
#include <unordered_set>
#include <set>
#include <limits>
#include <memory.h>

template<typename T> using Vector = std::vector<T>;
template<typename T> using Set = std::unordered_set<T>;
template<typename T, typename Comparator = std::less<T>> using OrderedSet = std::set<T, Comparator>;
template<typename T, int Capacity> using FixedVector = std::vector<T, StackAllocator<T, Capacity> >;

#endif //PLAZA_VECTOR_H
