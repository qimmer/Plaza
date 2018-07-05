//
// Created by Kim on 20/06/2018.
//

#include "NativeUtils.h"
#include "Entity.h"
#include "Debug.h"
#include <EASTL/unordered_map.h>
#include <EASTL/fixed_string.h>

typedef eastl::string LookupString;

API_EXPORT Entity GetUniqueEntity(StringRef name) {
    static eastl::unordered_map<LookupString, Entity> entityLookup;

    LookupString uniqueNameStr = name;

    auto it = entityLookup.find(uniqueNameStr);
    if(it == entityLookup.end()) {
        auto entity = entityLookup[uniqueNameStr] = CreateEntity();
        return entity;
    }

    return it->second;
}