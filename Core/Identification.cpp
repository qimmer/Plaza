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

struct Identification {
    StringRef Name, Uuid;
};

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

        char elementName[PathMax];
        switch(ownerPropertyKind) {
            case PropertyKind_Child:
                snprintf(elementName, PathMax, "%s", GetName(ownerProperty));
                entity = owner;
                break;
            case PropertyKind_Array:
            {
                unsigned long index = -1;
                auto elements = GetArrayPropertyElements(ownerProperty, owner);
                for(auto i = 0; i < GetArrayPropertyCount(ownerProperty, owner); ++i) {
                    if(elements[i] == entity) {
                        index = i;
                        break;
                    }
                }
                if(preferNamesToIndices) {
                    snprintf(elementName, PathMax, "%s/%s", GetName(ownerProperty), GetName(elements[index]));
                } else {
                    snprintf(elementName, PathMax, "%s/%lu", GetName(ownerProperty), index);
                }

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

API_EXPORT Entity FindEntityByName(Entity component, StringRef typeName) {
    typeName = Intern(typeName);
    for(auto i = 0; i < GetComponentMax(component); ++i) {
        auto entity = GetComponentEntity(component, i);
        if(IsEntityValid(entity) && typeName == GetName(entity)) {
            return entity;
        }
    }
    return 0;
}

API_EXPORT Entity FindEntityByPath(StringRef path) {
    while(path[0] == '/') path++; // Cut await leading slashes

    char pathSplits[PathMax];
    memset(pathSplits, 0, PathMax);
    strncpy(pathSplits, path, PathMax);

    auto len = strlen(pathSplits);

    // Split path into elements
    for(auto i = 0; i < len; ++i) {
        if(pathSplits[i] == '/') pathSplits[i] = '\0';
    }

    Entity currentEntity = GetModuleRoot();
    Entity currentArrayProperty = 0;

    char *element = pathSplits;
    while(element < pathSplits + len) {
        auto elementLength = strlen(element);
        auto internedElement = Intern(element);

        if(currentArrayProperty != 0) { // Path element is a name of one child element of the current array property
            auto childCount = GetArrayPropertyCount(currentArrayProperty, currentEntity);
            auto children = GetArrayPropertyElements(currentArrayProperty, currentEntity);

            for(auto i = 0; i < childCount; ++i) {
                if(internedElement == GetName(children[i])) {
                    currentArrayProperty = 0;
                    currentEntity = children[i];
                    break;
                }
            }

            if(currentArrayProperty) { // If still set, no matching child found by name. Try parse element as index
                char *endptr = NULL;
                auto index = strtol(element, &endptr, 10);
                if (element != endptr && index < childCount) {
                    currentArrayProperty = 0;
                    currentEntity = children[index];
                }
            }

            if(currentArrayProperty) { // If still set, we did not find a matching child
                return 0;
            }
        } else { // Path element is a property of the current entity
            auto foundProperty = FindEntityByName(ComponentOf_Property(), element);

            if(!IsEntityValid(foundProperty)) {
                Log(0, LogSeverity_Warning, "Property '%s' not found.", element);
                return 0;
            }

            switch(GetPropertyKind(foundProperty)) {
                case PropertyKind_Child:
                    GetPropertyValue(foundProperty, currentEntity, &currentEntity);
                    if(!IsEntityValid(currentEntity)) {
                        Log(0, LogSeverity_Warning, "Entity '%s' has no %s.", GetName(currentEntity), element);
                        return 0;
                    }
                    break;
                case PropertyKind_Array:
                    currentArrayProperty = foundProperty;
                    break;
            }
        }

        element += elementLength + 1;
    }

    if(currentArrayProperty != 0) {
        Log(0, LogSeverity_Error, "Incomplete entity path: Path '%s' ended with array property.", path);
        return 0;
    }

    return currentEntity;
}


API_EXPORT Entity FindEntityByUuid(StringRef uuid) {
    if(strlen(uuid) == 0) {
        return 0;
    }

    uuid = Intern(uuid);

    for_entity(entity, data, Identification) {
        if(data->Uuid == uuid) {
            return entity;
        }
    }

    return 0;
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
    __Property(PropertyOf_Name(), offsetof(Identification, Name), sizeof(Identification::Name), TypeOf_StringRef,  component, 0, 0);
    __Property(PropertyOf_Uuid(), offsetof(Identification, Uuid), sizeof(Identification::Uuid), TypeOf_StringRef,  component, 0, 0);
}

API_EXPORT StringRef GetName(Entity entity)  {
    auto data = GetIdentificationData(entity);
	if (!data) {
		return "";
	}

    return data->Name;
}

API_EXPORT void SetName(Entity entity, StringRef name) {
    AddComponent(entity, ComponentOf_Identification());

    GetUuid(entity); // Provoke eventual new Guid

    auto data = GetIdentificationData(entity);

    name = Intern(name);

    if(data->Name != name) {
        auto oldName = data->Name;

        data->Name = AddStringRef(name);
        EmitChangedEvent(entity, TypeOf_StringRef, PropertyOf_Name(), &oldName, &name);

        ReleaseStringRef(oldName);

    }
}

API_EXPORT StringRef GetUuid(Entity entity)  {
    if(!IsEntityValid(entity)) {
        return "";
    }

    auto data = GetIdentificationData(entity);
    if (!data || !data->Uuid || !data->Uuid[0]) {

        auto uniqueName = GetUniqueEntityName(entity);
        if(uniqueName) {
            SetUuid(entity, uniqueName);
        } else {
            SetUuid(entity, CreateGuid());
        }

        data = GetIdentificationData(entity);
    }

    return data->Uuid;
}

API_EXPORT void SetUuid(Entity entity, StringRef value) {
    AddComponent(entity, ComponentOf_Identification());

    value = Intern(value);

    auto data = GetIdentificationData(entity);

    if(!value || !strlen(value)) {
        Log(entity, LogSeverity_Error, "Uuid '%s' is invalid. Uuid has not changed.", value);
        return;
    }

    if(data->Uuid != value) {
        auto existing = FindEntityByUuid(value);
        if(IsEntityValid(existing)) {
            Log(entity, LogSeverity_Error, "Uuid '%s' is already occupied by '%s'. Uuid has not changed.", value, GetDebugName(existing));
            return;
        }

        auto oldValue = data->Uuid;

        EmitChangedEvent(entity, TypeOf_StringRef, PropertyOf_Uuid(), &data->Uuid, &value);

        ReleaseStringRef(oldValue);
        data->Uuid = AddStringRef(value);
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

BeginUnit(Identification)
    BeginComponent(Identification)
        RegisterProperty(StringRef, Name)
        RegisterProperty(StringRef, Uuid)
    EndComponent()
EndUnit()
