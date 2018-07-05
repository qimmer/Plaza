#include <Core/Hierarchy.h>
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

struct Event {
    Vector(HandlerFunctions, Entity, 32);
    Vector(EventArgumentCache, EventArgument*, 8);
};

struct Hierarchy {
    Entity Parent;
    Entity FirstChild;
    Entity Sibling;
    bool IsLocked;

    char Name[256 - (sizeof(Entity) * 3) - 1];

    char Path[1024 - 256];
};

static eastl::unordered_map<eastl::string, Entity> EntityPathMap;
std::map<Entity, eastl::string> EntityMap;

static Entity FirstRoot = 0;

static void CalculateEntityPath(char *dest, size_t bufMax, Entity entity) {
    char path[PATH_MAX];
    char path2[PATH_MAX];
    snprintf(path, PATH_MAX, "");
    snprintf(path2, PATH_MAX, "");

    while(IsEntityValid(entity)) {
        snprintf(path2, PATH_MAX, "/%s%s", GetName(entity), path);
        entity = GetParent(entity);
        strncpy(path, path2, PATH_MAX);
    }

    strncpy(dest, path2, bufMax);
}

static void GetParentPath(char *dest, u32 maxSize, StringRef path) {
    char *lastSep;
    strncpy(dest, path, maxSize);
    lastSep = strrchr(dest, '/');
    if(lastSep != NULL) {
        *lastSep = '\0';
    }
}


API_EXPORT Entity FindEntityByName(Entity component, StringRef typeName) {
    for(auto i = 0; i < GetNumComponents(component); ++i) {
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

API_EXPORT Entity GetFirstChild(Entity parent) {
        // If parent is zero, return first root entity
        if(parent == 0) {
            return FirstRoot;
        }

        auto data = GetHierarchy(parent);
        if(!data) return 0;

        return data->FirstChild;
    }

API_EXPORT Entity GetSibling(Entity child) {
    if(!IsEntityValid(child)) return 0;

    auto data = GetHierarchy(child);
    if(!data) return 0;

    return data->Sibling;
}

API_EXPORT Entity FindChild(Entity parent, StringRef childName) {
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", GetEntityPath(parent), childName);

    return FindEntityByPath(path);
}

API_EXPORT StringRef GetEntityPath(Entity entity) {
        return GetHierarchy(entity)->Path;
    }

API_EXPORT Entity CreateEntityFromPath(StringRef path) {
        Assert(0, path);
        if(path[0] != '/') {
            Log(0, LogSeverity_Error, "Absolute entity path is malformed: %s", path);
            return 0;
        }

        auto existing = FindEntityByPath(path);
        if(IsEntityValid(existing)) {
            return existing;
        }

        StringRef name = strrchr(path, '/');
        Assert(0, name);
		name++;

        char parentPath[PATH_MAX];
        GetParentPath(parentPath, PATH_MAX, path);

        auto parent = parentPath[0] ? CreateEntityFromPath(parentPath) : 0;

        return CreateEntityFromName(parent, name);
    }

API_EXPORT Entity CreateEntityFromName(Entity parent, StringRef name) {
    Assert(0, name);

    auto nameLen = strlen(name);
    if(nameLen <= 0) {
        Log(0, LogSeverity_Error, "Entity name cannot be empty");
        return 0;
    }

    for(auto i = 0; i < nameLen; ++i) {
        if(name[i] == ':' || name[i] == '/' || name[i] == '\\') {
            Log(0, LogSeverity_Error, "Entity name '%s' contains invalid character: '%c'", name, name[i]);
            return 0;
        }
    }

    for_children(child, parent) {
        if(strcmp(GetName(child), name) == 0) {
            return child;
        }
    }

    auto entity = CreateEntity();
    SetName(entity, name);
    SetParent(entity, parent);
    return entity;
}

API_EXPORT bool IsEntityDecendant(Entity entity, Entity parent) {
        Assert(entity, IsEntityValid(entity));

        if(!HasComponent(entity, ComponentOf_Hierarchy()) || !IsEntityValid(parent)) {
            return false;
        }

        while(IsEntityValid(entity)) {
            if(entity == parent) {
                return true;
            }

            entity = GetParent(entity);
        }

        return false;
    }

API_EXPORT Entity GetNextChild(Entity parent, Entity currentChild) {
    if(!currentChild) return GetFirstChild(parent);
    return GetSibling(currentChild);
}

API_EXPORT Entity CopyEntity(Entity templateEntity, StringRef copyPath) {
    char buffer[128];

    auto copy = CreateEntityFromPath(copyPath);

    for(auto i = 0; i < GetNumComponents(ComponentOf_Component()); ++i) {
        auto component = GetComponentEntity(ComponentOf_Component(), i);

        if(HasComponent(templateEntity, component)) {
            for(auto j = 0; j < GetNumComponents(ComponentOf_Property()); ++j) {
                auto property = GetComponentEntity(ComponentOf_Property(), j);

                if(GetPropertyComponent(property) != component) continue;

                GetPropertyValue(property, templateEntity, buffer);
                SetPropertyValue(property, copy, buffer);
            }
        }
    }

    return copy;
}

static void Detach(Entity entity, Entity parent) {
    if(IsEntityValid(parent)) {
        if(GetFirstChild(parent) == entity) {
            GetHierarchy(parent)->FirstChild = GetHierarchy(entity)->Sibling;
        } else {
            for(auto child = GetFirstChild(parent); child; child = GetSibling(child)) {
                auto sibling = GetSibling(child);
                if(sibling == entity) {
                    GetHierarchy(child)->Sibling = GetHierarchy(sibling)->Sibling;
                    break;
                }
            }
        }
    } else {
        if(FirstRoot == entity) {
            FirstRoot = GetHierarchy(entity)->Sibling;
        } else {
            for(auto child = FirstRoot; child; child = GetSibling(child)) {
                auto sibling = GetSibling(child);
                if(sibling == entity) {
                    GetHierarchy(child)->Sibling = GetHierarchy(sibling)->Sibling;
                    break;
                }
            }
        }
    }

    auto data = GetHierarchy(entity);
    data->Sibling = 0;
    EntityPathMap.erase(data->Path);
    EntityMap.erase(entity);
}

bool IsEntityAncestorOf(Entity entity, Entity parent) {
    if(!IsEntityValid(entity) || !IsEntityValid(parent)) return false;

    if(entity == parent) return true;

    bool result = false;
    for_children(child, parent) {
        result |= IsEntityAncestorOf(entity, child);

        if(result) break;
    }

    return result;
}

static void Attach(Entity entity, Entity parent) {
    if(IsEntityValid(parent)) {
        if(IsEntityAncestorOf(parent, entity)) {
            Log(entity, LogSeverity_Error, "Entity cannot attach to any of it's children");
            return;
        }

        auto child = GetFirstChild(parent);
        if(!IsEntityValid(child)) {
            Assert(entity, parent != entity);
            GetHierarchy(parent)->FirstChild = entity;
        } else {
            while(GetSibling(child)) {
                child = GetSibling(child);
            }

            Assert(entity, child != entity);
            GetHierarchy(child)->Sibling = entity;
        }
    } else {
        auto child = FirstRoot;
        if(!IsEntityValid(child)) {
            FirstRoot = entity;
        } else {
            while(GetSibling(child)) {
                child = GetSibling(child);
            }

            Assert(entity, child != entity);
            GetHierarchy(child)->Sibling = entity;
        }
    }

    auto data = GetHierarchy(entity);
    CalculateEntityPath(data->Path, sizeof(data->Path), entity);

    EntityPathMap[data->Path] = entity;
    EntityMap[entity] = data->Path;
}

static void OnNameChanged(Entity entity, StringRef oldName, StringRef newName) {
    char newPath[PATH_MAX];
    CalculateEntityPath(newPath, PATH_MAX, entity);
    auto existingEntity = FindEntityByPath(newPath);

    Assert(entity, !IsEntityValid(existingEntity) || existingEntity == entity);

    auto data = GetHierarchy(entity);
    EntityPathMap.erase(data->Path);
    EntityMap.erase(entity);
    strcpy(data->Path, newPath);
    EntityPathMap[data->Path] = entity;
    EntityMap[entity] = data->Path;
}

API_EXPORT void SetParent(Entity entity, Entity parent) {
    static Entity prop = PropertyOf_Parent();
    Entity oldParent = 0;
    GetPropertyValue(prop, entity, &oldParent);

    AddComponent(entity, ComponentOf_Hierarchy());
    if(IsEntityValid(parent)) AddComponent(parent, ComponentOf_Hierarchy());

    SetPropertyValue(prop, entity, &parent);

    Detach(entity, oldParent);
    Attach(entity, parent);
}


API_EXPORT void SetName(Entity entity, StringRef name) {
    static Entity prop = PropertyOf_Name();
    StringRef oldName = 0;
    GetPropertyValue(prop, entity, &oldName);

    AddComponent(entity, ComponentOf_Hierarchy());

    SetPropertyValue(prop, entity, &name);

    OnNameChanged(entity, oldName, name);
}


LocalFunction(OnHierarchyRemoved, void, Entity entity) {
    while(GetFirstChild(entity)) {
        DestroyEntity(GetFirstChild(entity));
    }

    Detach(entity, GetParent(entity));
}

LocalFunction(OnHierarchyAdded, void, Entity entity) {
    Attach(entity, 0); // Attach to hierarchy as a root entity
}


BeginUnit(Hierarchy)
    BeginComponent(Hierarchy)
        RegisterProperty(Entity, Parent)
        RegisterProperty(StringRef, Name)
        RegisterProperty(bool, IsLocked)
    EndComponent()

    RegisterSubscription(HierarchyAdded, OnHierarchyAdded)
    RegisterSubscription(HierarchyRemoved, OnHierarchyRemoved)

    __ForceArgumentParse(EventOf_ParentChanged());
EndUnit()

void __InitializeHierarchy() {
    auto component = ComponentOf_Hierarchy();
    __Property(PropertyOf_Name(), offsetof(Hierarchy, Name), sizeof(Hierarchy::Name), TypeOf_StringRef,  component);
    __Property(PropertyOf_Parent(), offsetof(Hierarchy, Parent), sizeof(Hierarchy::Parent), TypeOf_Entity,  component);
}

static void PrintHierarchy(int level, Entity entity) {
    static const int identation = 4;

    printf("%*s - %s (i: %d)\n", level * identation, " ", GetName(entity), GetEntityIndex(entity));

    for_children(child, entity) {
        PrintHierarchy(level + 1, child);
    }
}

API_EXPORT void DumpHierarchy() {
    for(auto entity = GetNextEntity(0); IsEntityValid(entity); entity = GetNextEntity(entity)) {
        if(!HasComponent(entity, ComponentOf_Hierarchy()) || !IsEntityValid(GetParent(entity))) {
            PrintHierarchy(0, entity);
        }
    }
}
