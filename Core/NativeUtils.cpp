//
// Created by Kim on 20/06/2018.
//

#include "NativeUtils.h"
#include "Entity.h"
#include "Debug.h"
#include "Hashing.h"
#include "Identification.h"
#include <EASTL/unordered_map.h>
#include <EASTL/map.h>
#include <EASTL/fixed_string.h>

#include <string>
#include <unordered_map>

static eastl::unordered_map<eastl::string, Entity> nameLookup;
static eastl::map<Entity, eastl::string> entityLookup;

#define Verbose_Entity "entity"

API_EXPORT Entity GetUniqueEntity(StringRef name, bool *firstTime) {
    auto it = nameLookup.find(name);
    if(it == nameLookup.end()) {
        auto entity = nameLookup[name] = CreateEntity();
		entityLookup[entity] = name;
        if(firstTime) *firstTime = true;
        Verbose(Verbose_Entity, "Unique Entity %s Created: %s", name, GetDebugName(entity));

        return entity;
    }

    if(firstTime) *firstTime = false;

    return it->second;
}

API_EXPORT StringRef GetUniqueEntityName(Entity entity) {
	auto it = entityLookup.find(entity);
	if (it != entityLookup.end()) {
		return Intern(it->second.c_str());
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