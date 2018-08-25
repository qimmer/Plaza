//
// Created by Kim on 20/06/2018.
//

#include "NativeUtils.h"
#include "Entity.h"
#include "Debug.h"
#include <EASTL/unordered_map.h>
#include <EASTL/fixed_string.h>

typedef eastl::string LookupString;

static eastl::unordered_map<LookupString, Entity> entityLookup;

API_EXPORT Entity GetUniqueEntity(StringRef name, bool *firstTime) {
    LookupString uniqueNameStr = name;

    auto it = entityLookup.find(uniqueNameStr);
    if(it == entityLookup.end()) {
        auto entity = entityLookup[uniqueNameStr] = __CreateEntity();
        if(firstTime) *firstTime = true;
        Verbose(VerboseLevel_ComponentEntityCreationDeletion, "Unique Entity %s Created: %s", name, GetDebugName(entity));
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