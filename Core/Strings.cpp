//
// Created by Kim on 15-09-2018.
//

#include "Strings.h"
#include <Core/Hashing.h>

#include <unordered_map>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_string.h>

typedef eastl::string SmallString;
typedef eastl::fixed_hash_map<SmallString, u32, 1024> StringMap;

StringMap lookup;

static StringRef nullString = "";

API_EXPORT StringRef AddStringRef(StringRef sourceString) {
    if(!sourceString || !sourceString[0]) return nullString;

    auto it = lookup.find(sourceString);
    if(it == lookup.end()) {
        auto result = lookup.insert(eastl::pair<SmallString, u32>(sourceString, 1));
        return result.first->first.c_str();
    }

    it->second++;

    return it->first.c_str();
}

API_EXPORT void ReleaseStringRef(StringRef sourceString) {
    if(!sourceString || !sourceString[0]) return;

    auto it = lookup.find(sourceString);
    if(it != lookup.end() && it->second > 0) {
        it->second--;
    }
}

API_EXPORT StringRef Intern(StringRef sourceString) {
    if(!sourceString || !sourceString[0]) return nullString;

    auto it = lookup.find(sourceString);
    if(it == lookup.end()) {
        auto result = lookup.insert(eastl::pair<SmallString, u32>(sourceString, 0));
        return result.first->first.c_str();
    }

    return it->first.c_str();
}

API_EXPORT void CleanupStrings() {
    for(auto it = lookup.begin(); it != lookup.end();) {
        if(it->second == 0) {
            auto oldIt = it;
            ++it;

            lookup.erase(oldIt);
        } else {
            ++it;
        }
    }
}