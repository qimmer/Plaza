//
// Created by Kim on 18-07-2018.
//

#include "Identification.h"
#include <Core/Event.h>
#include <climits>
#include <Core/Entity.h>
#include <map>
#include <EASTL/unordered_map.h>
#include <EASTL/fixed_string.h>
#include "Component.h"
#include "Debug.h"
#include "Property.h"
#include "Vector.h"
#include "Math.h"

struct Identification {
    char Name[256];
};

API_EXPORT void CalculateEntityPath(char *dest, size_t bufMax, Entity entity) {
    typedef char Path[PathMax];
    Path paths[2];

    snprintf(paths[0], PathMax, "");
    snprintf(paths[1], PathMax, "");

    int currentPath = 0;

    while(IsEntityValid(entity) && entity != GetModuleRoot()) {
        auto owner = GetOwner(entity);
        auto ownerProperty = GetOwnerProperty(entity);

        if(!IsEntityValid(owner)) {
            Log(entity, LogSeverity_Error, "Entity has no owner: %s", GetDebugName(entity));
            return;
        }

        if(!IsEntityValid(ownerProperty)) {
            Log(entity, LogSeverity_Error, "Entity has no owner property: %s", GetDebugName(entity));
            return;
        }

        auto ownerPropertyKind = GetPropertyKind(ownerProperty);

        char elementName[PathMax];
        switch(ownerPropertyKind) {
            case PropertyKind_Child:
                snprintf(elementName, PathMax, "%s", GetName(ownerProperty));
                entity = owner;
                break;
            case PropertyKind_Array:
                snprintf(elementName, PathMax, "%s/%s", GetName(ownerProperty), GetName(entity));
                entity = owner;
                break;
            default:
                Assert(entity, false);
                break;
        }

        snprintf(paths[currentPath], PathMax, "/%s%s", elementName, paths[1 - currentPath]);
        currentPath = 1 - currentPath;
    }

    StringRef finalPath = paths[1 - currentPath];
    strncpy(dest, finalPath, bufMax);
}

API_EXPORT Entity FindEntityByName(Entity component, StringRef typeName) {
    for(auto i = 0; i < GetComponentMax(component); ++i) {
        auto entity = GetComponentEntity(component, i);
        if(strcmp(GetName(entity), typeName) == 0) {
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

        if(currentArrayProperty != 0) { // Path element is a name of one child element of the current array property
            auto childCount = GetArrayPropertyCount(currentArrayProperty, currentEntity);
            auto children = GetArrayPropertyElements(currentArrayProperty, currentEntity);
            for(auto i = 0; i < childCount; ++i) {
                if(strcmp(GetName(children[i]), element) == 0) {
                    currentArrayProperty = 0;
                    currentEntity = children[i];
                    break;
                }
            }

            if(currentArrayProperty) { // If still set, we did not find a matching child
                Log(0, LogSeverity_Info, "%s '%s' not found on entity '%s'.", GetName(currentArrayProperty), element, GetName(currentEntity));
                return 0;
            }
        } else { // Path element is a property of the current entity
            auto foundProperty = FindEntityByName(ComponentOf_Property(), element);

            if(!IsEntityValid(foundProperty)) {
                Log(0, LogSeverity_Error, "Property '%s' not found.", element);
                return 0;
            }

            switch(GetPropertyKind(foundProperty)) {
                case PropertyKind_Child:
                    GetPropertyValue(foundProperty, currentEntity, &currentEntity);
                    if(!IsEntityValid(currentEntity)) {
                        Log(0, LogSeverity_Error, "Entity '%s' has no %s.", GetName(currentEntity), element);
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

API_EXPORT StringRef GetEntityRelativePath(StringRef entityPath, Entity relativeTo) {
    char parentPath[2048];
    CalculateEntityPath(parentPath, 2048, relativeTo);

    if(memcmp(entityPath, parentPath, Min(strlen(parentPath), strlen(entityPath))) != 0) {
        // Entity in entityPath is not a descendant of relativeTo. Return the absolute path given.
        return entityPath;
    }

    auto relativePath = entityPath + strlen(parentPath);
    if(relativePath[0] == '/') relativePath++; // Remove eventual preceeding slash when path is relative

    return relativePath;
}

API_EXPORT Entity CopyEntity(Entity templateEntity, Entity destinationEntity) {
    char buffer[128];

    auto copy = CreateEntity();

    for(auto i = 0; i < GetComponentMax(ComponentOf_Component()); ++i) {
        auto component = GetComponentEntity(ComponentOf_Component(), i);

        if(HasComponent(templateEntity, component)) {
            for(auto j = 0; j < GetComponentMax(ComponentOf_Property()); ++j) {
                auto property = GetComponentEntity(ComponentOf_Property(), j);

                if(GetOwner(property) != component) continue;

                GetPropertyValue(property, templateEntity, buffer);
                SetPropertyValue(property, copy, buffer);
            }
        }
    }

    return copy;
}

API_EXPORT void SetName(Entity entity, StringRef name) {
    AddComponent(entity, ComponentOf_Identification());

    auto data = GetIdentificationData(entity);
    if(strcmp(data->Name, name) != 0) {
        char oldName[sizeof(Identification::Name)];

        strncpy(oldName, data->Name, sizeof(data->Name));
        strncpy(data->Name, name, sizeof(data->Name));

        const Type argumentTypes[] = {TypeOf_Entity, TypeOf_StringRef, TypeOf_StringRef};
        const void * argumentData[] = {&entity, &oldName, &name};

        auto changedEvent = GetPropertyChangedEvent(PropertyOf_Name());
        FireEventFast(changedEvent, 3, argumentTypes, argumentData);
    }
}

BeginUnit(Identification)
    BeginComponent(Identification)
        RegisterProperty(StringRef, Name)
    EndComponent()
EndUnit()

void __InitializeNode() {
    auto component = ComponentOf_Identification();
    __Property(PropertyOf_Name(), offsetof(Identification, Name), sizeof(Identification::Name), TypeOf_StringRef,  component, 0, 0);
}

API_EXPORT StringRef GetName(Entity entity)  {
    auto data = GetIdentificationData(entity);
    return data->Name;
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
