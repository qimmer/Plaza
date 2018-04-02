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
#include <Core/AlignedAllocator.h>

#define ArenaSize 4096

template<typename T> using Vector = std::vector<T/*, AlignedAllocator<T, alignof(T)>*/>;
template<typename T> using Set = std::unordered_set<T, std::hash<T>, std::equal_to<T>/*, AlignedAllocator<T, alignof(T)>*/>;
template<typename T, typename Comparator = std::less<T>> using OrderedSet = std::set<T, Comparator/*, AlignedAllocator<T, alignof(T)>*/>;

#endif //PLAZA_VECTOR_H
