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
    if(!sourceString || !sourceString[0]) return 0;

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
    if(!sourceString || !sourceString[0]) return 0;

    auto it = lookup.find(sourceString);
    if(it == lookup.end()) {

        auto result = lookup.insert(eastl::pair<SmallString, u32>(sourceString, 0));
        return result.first->first.c_str();
    }

    return it->first.c_str();
}

#define FORMAT_CASE(TYPE) case TypeOf_ ## TYPE: return StringFormatV(format, argument.as_ ## TYPE); break;

API_EXPORT StringRef Format(StringRef format, Variant argument) {
    switch(argument.type) {
        FORMAT_CASE(s8)
        FORMAT_CASE(s16)
        FORMAT_CASE(s32)
        FORMAT_CASE(s64)
        FORMAT_CASE(u8)
        FORMAT_CASE(u16)
        FORMAT_CASE(u32)
        FORMAT_CASE(u64)
        FORMAT_CASE(StringRef)
        FORMAT_CASE(bool)
        FORMAT_CASE(float)
        FORMAT_CASE(double)
        FORMAT_CASE(Entity)
        default:
            return format;
    }
}

API_EXPORT StringRef StringFormatV(StringRef format, ...) {
    char buffer[4096];
    auto numWritten = 0;

    va_list arg;
    va_start(arg, format);
    numWritten += vsnprintf(buffer + numWritten, 4095, format, arg);
    va_end(arg);

    buffer[numWritten] = '\0';

    return Intern(buffer);
}
