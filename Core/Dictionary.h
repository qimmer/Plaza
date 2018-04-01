//
// Created by Kim Johannsen on 03/01/2018.
//

#ifndef PLAZA_DICTIONARY_H
#define PLAZA_DICTIONARY_H

#include <unordered_map>
#include <map>

template<typename K, typename V> using Dictionary = std::unordered_map<K, V>;
template<typename K, typename V> using OrderedDictionary = std::map<K, V>;

#endif //PLAZA_DICTIONARY_H
