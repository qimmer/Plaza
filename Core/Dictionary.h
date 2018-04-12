//
// Created by Kim Johannsen on 03/01/2018.
//

#ifndef PLAZA_DICTIONARY_H
#define PLAZA_DICTIONARY_H

#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_map.h>
#include <Core/String.h>

template<typename V> using Dictionary = eastl::fixed_hash_map<String, V, 128, 129, true, eastl::string_hash<String>>;
template<typename K, typename V> using Lookup = eastl::fixed_hash_map<K, V, 128>;

#endif //PLAZA_DICTIONARY_H
