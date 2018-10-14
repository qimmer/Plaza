//
// Created by Kim on 15-09-2018.
//

#include "Strings.h"
#include <Core/Hashing.h>

#include <unordered_map>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_string.h>
#include <EASTL/unordered_map.h>

typedef eastl::string SmallString;
typedef eastl::unordered_map<SmallString, u32> StringMap;

StringMap lookup;

static StringRef nullString = "";

u32 lastCleanupSize = 0;

API_EXPORT void CleanupStrings() {
    if(lookup.size() > (lastCleanupSize + 1024)) {
        for(auto it = lookup.begin(); it != lookup.end();) {
            if(it->second == 0) {
                auto oldIt = it;
                ++it;

                lookup.erase(oldIt);
            } else {
                ++it;
            }
        }

        lastCleanupSize = lookup.size();
    }
}

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
