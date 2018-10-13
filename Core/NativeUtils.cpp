//
// Created by Kim on 20/06/2018.
//

#include "NativeUtils.h"
#include "Entity.h"
#include "Debug.h"
#include "Hashing.h"
#include "Identification.h"
#include <EASTL/unordered_map.h>
#include <EASTL/fixed_string.h>

#include <string>
#include <unordered_map>

typedef std::string LookupString;

static std::unordered_map<LookupString, Entity> entityLookup;

#define Verbose_Entity "entity"

API_EXPORT Entity GetUniqueEntity(StringRef name, bool *firstTime) {
    LookupString uniqueNameStr = name;

    auto it = entityLookup.find(uniqueNameStr);
    if(it == entityLookup.end()) {
        auto entity = entityLookup[uniqueNameStr] = CreateEntity();
        if(firstTime) *firstTime = true;
        Verbose(Verbose_Entity, "Unique Entity %s Created: %s", name, GetDebugName(entity));

        return entity;
    }

    if(firstTime) *firstTime = false;

    return it->second;
}

API_EXPORT StringRef GetUniqueEntityName(Entity entity) {
    for(auto& it : entityLookup) {
        if(it.second == entity) {
            return it.first.c_str();
        }
    }

    return NULL;
}

API_EXPORT void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return new char[size];
}

API_EXPORT void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return new char[size];
}