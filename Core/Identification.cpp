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

static eastl::unordered_multimap<eastl::string, Entity> EntityPathMap;

static Entity FirstRoot = 0;

API_EXPORT void CalculateEntityPath(char *dest, size_t bufMax, Entity entity) {
    char path[PathMax];
    char path2[PathMax];
    snprintf(path, PathMax, "");
    snprintf(path2, PathMax, "");

    while(IsEntityValid(entity)) {
        auto owner = GetOwner(entity);
        for_entity(component, componentData, Component) {
            if(HasComponent(entity, component)) {
                auto properties = GetProperties(component);
                for(auto i = 0; i < GetNumProperties(component); ++i) {
                    auto property = properties[i];
                    auto kind = GetPropertyKind(property);

                    if(kind == PropertyKind_Child) {
                        Entity value = 0;
                        GetPropertyValue(property, owner, &value);
                        if(value == entity) {
                            snprintf(path2, PathMax, "/%s%s", GetName(property), path);
                            entity = owner;
                            continue;
                        }
                    }

                    else if(kind == PropertyKind_Array) {
                        auto count = GetArrayPropertyCount(property, owner);
                        for(auto i = 0; i < count; ++i) {
                            auto element = GetArrayPropertyElement(property, owner, i);
                            if(element == entity) {
                                snprintf(path2, PathMax, "/%s/%s%s", GetName(property), GetName(element), path);
                                entity = owner;
                                break;
                            }
                        }
                    }
                }
            }
        }
        entity =

        snprintf(path2, PathMax, "/%s%s", GetName(entity), path);

        strncpy(path, path2, PathMax);
    }

    strncpy(dest, path2, bufMax);
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
    auto it = EntityPathMap.find(path);
    if(it == EntityPathMap.end()) return 0;

    return it->second;
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

API_EXPORT Entity CopyEntity(Entity templateEntity, StringRef copyPath) {
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
