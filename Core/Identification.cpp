//
// Created by Kim on 18-07-2018.
//

#include "Identification.h"
#include <Core/Event.h>
#include <Core/Entity.h>

#include "Component.h"
#include "Debug.h"
#include "Property.h"
#include "Vector.h"
#include "Algorithms.h"
#include "Hashing.h"
#include "Std.h"
#include "Strings.h"

#include <climits>
#include <map>
#include <EASTL/unordered_map.h>
#include <EASTL/fixed_string.h>

#ifdef WIN32
#undef Enum
#include <combaseapi.h>
#else
#include <uuid/uuid.h>
#endif

static std::map<const void*, Entity> uuidTable;

API_EXPORT StringRef CalculateEntityPath(Entity entity, bool preferNamesToIndices) {
    typedef char Path[PathMax];
    Path paths[2];

    snprintf(paths[0], PathMax, "%s", "");
    snprintf(paths[1], PathMax, "%s", "");

    int currentPath = 0;

    while(IsEntityValid(entity) && entity != GetModuleRoot()) {
        auto owner = GetOwner(entity);
        auto ownerProperty = GetOwnerProperty(entity);

        if(!IsEntityValid(owner)) {
            Log(entity, LogSeverity_Error, "Entity has no owner: %s", GetDebugName(entity));
            return 0;
        }

        if(!IsEntityValid(ownerProperty)) {
            Log(entity, LogSeverity_Error, "Entity has no owner property: %s", GetDebugName(entity));
            return 0;
        }

        auto ownerPropertyKind = GetPropertyKind(ownerProperty);
        auto ownerPropertyName = strrchr(GetUuid(ownerProperty), '.') + 1;

        char elementName[PathMax];
        switch(ownerPropertyKind) {
            case PropertyKind_Child:
                snprintf(elementName, PathMax, "%s", ownerPropertyName);
                entity = owner;
                break;
            case PropertyKind_Array:
            {
                unsigned long index = -1;

                auto& elements = GetArrayPropertyElements(ownerProperty, owner);
                for(u32 i = 0; i < elements.size(); ++i) {
                    if(elements[i] == entity) {
                        index = i;
                        break;
                    }
                }

                snprintf(elementName, PathMax, "%s[%lu]", ownerPropertyName, index);

                entity = owner;
            }

                break;
            default:
                Assert(entity, false);
                break;
        }

        snprintf(paths[currentPath], PathMax, "/%s%s", elementName, paths[1 - currentPath]);
        currentPath = 1 - currentPath;
    }

    StringRef finalPath = paths[1 - currentPath];
    return Intern(finalPath);
}

API_EXPORT Entity FindEntityByName(Entity component, StringRef name) {
    name = Intern(name);
    for_entity(identification, data, Identification) {
        if(name == data->Name && HasComponent(identification, component)) {
            return identification;
        }
    }
    
    return 0;
}


API_EXPORT Entity FindEntityByUuid(StringRef uuid) {
    if(!uuid || strlen(uuid) == 0) {
        return 0;
    }

    uuid = Intern(uuid);

    auto it = uuidTable.find(uuid);
    if(it == uuidTable.end()) {
        return 0;
    }

    return it->second;
}

API_EXPORT StringRef GetEntityRelativePath(StringRef entityPath, Entity relativeTo, bool preferNameToIndex) {
    auto parentPath = CalculateEntityPath(relativeTo, preferNameToIndex);

    if(memcmp(entityPath, parentPath, Min(strlen(parentPath), strlen(entityPath))) != 0) {
        // Entity in entityPath is not a descendant of relativeTo. Return the absolute path given.
        return entityPath;
    }

    auto relativePath = entityPath + strlen(parentPath);
    if(relativePath[0] == '/') relativePath++; // Remove eventual preceeding slash when path is relative

    return relativePath;
}

void __InitializeNode() {
    auto component = ComponentOf_Identification();
	SetUuid(component, "Component.Identification");
    __Property(PropertyOf_Uuid(), offsetof(Identification, Uuid), sizeof(Identification::Uuid), TypeOf_StringRef,  component, 0, 0, "Uuid");
}

API_EXPORT StringRef GetUuid(Entity entity)  {
    if(!IsEntityValid(entity)) {
        return 0;
    }

    auto data = GetIdentificationData(entity);
    if (!data || !data->Uuid || !data->Uuid[0]) {

		return 0;
    }

    return data->Uuid;
}

API_EXPORT void SetUuid(Entity entity, StringRef value) {
    AddComponent(entity, ComponentOf_Identification());

    value = Intern(value);

    auto data = GetIdentificationData(entity);

    if(data->Uuid != value) {
        auto newUuidIt = uuidTable.find(value);
        if(newUuidIt != uuidTable.end() && newUuidIt->second != entity) {
            Log(entity, LogSeverity_Error, "Uuid '%s' is already occupied by '%s'. Uuid has not changed.", value, GetDebugName(newUuidIt->second));
            return;
        }

        auto oldValue = data->Uuid;

        auto oldIt = uuidTable.find(oldValue);
        if(oldIt != uuidTable.end()) {
            uuidTable.erase(oldIt);
        }

        uuidTable[value] = entity;
        ReleaseStringRef(oldValue);
        data->Uuid = AddStringRef(value);

        EmitChangedEvent(entity, PropertyOf_Uuid(), GetPropertyData(PropertyOf_Uuid()), MakeVariant(StringRef, oldValue), MakeVariant(StringRef, value));
    }
}

LocalFunction(OnRemoved, void, Entity component, Entity entity) {
	auto data = GetIdentificationData(entity);
	auto oldValue = data->Uuid;

	auto oldIt = uuidTable.find(oldValue);
	if (oldIt != uuidTable.end()) {
		uuidTable.erase(oldIt);
	}
}

API_EXPORT bool GetParentPath(StringRef childPath, u32 bufLen, char *parentPath) {
    strncpy(parentPath, childPath, bufLen);

    auto len = strlen(childPath);
    while(len > 0 && parentPath[len] != '/') {
        len--;
    }

    if(parentPath[len] != '/' && len > 0) {
        return false;
    }

    parentPath[len] = '\0';

    return true;
}

API_EXPORT StringRef CreateGuid() {
    char buf[128];
    memset(buf, 0, sizeof(buf));

#ifdef WIN32
    GUID guid;
    CoCreateGuid(&guid);

    snprintf(buf, 128, "%08lx-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
           guid.Data1, guid.Data2, guid.Data3,
           guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
           guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
#endif

    return Intern(buf);
}

API_EXPORT StringRef CreateGuidFromPath(StringRef path) {

    return nullptr;
}

LocalFunction(OnUuidChanged, void, Entity entity, StringRef oldValue, StringRef newValue) {
    auto oldUuidIt = uuidTable.find(oldValue);
    if(oldUuidIt != uuidTable.end()) uuidTable.erase(oldUuidIt);
    uuidTable[newValue] = entity;
}

BeginUnit(Identification)
    BeginComponent(Identification)
        RegisterProperty(StringRef, Uuid)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Uuid()), OnUuidChanged, 0)
	RegisterSubscription(EventOf_EntityComponentRemoved(), OnRemoved, ComponentOf_Identification())
EndUnit()
