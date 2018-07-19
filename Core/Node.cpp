#include <Core/Node.h>
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

struct Node {
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
    char path[PathMax];
    char path2[PathMax];
    snprintf(path, PathMax, "");
    snprintf(path2, PathMax, "");

    while(IsEntityValid(entity)) {
        snprintf(path2, PathMax, "/%s%s", GetName(entity), path);
        entity = GetParent(entity);
        strncpy(path, path2, PathMax);
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

API_EXPORT Entity GetFirstChild(Entity parent) {
        // If parent is zero, return first root entity
        if(parent == 0) {
            return FirstRoot;
        }

        auto data = GetNodeData(parent);
        if(!data) return 0;

        return data->FirstChild;
    }

API_EXPORT Entity GetSibling(Entity child) {
    if(!IsEntityValid(child)) return 0;

    auto data = GetNodeData(child);
    if(!data) return 0;

    return data->Sibling;
}

API_EXPORT Entity FindChild(Entity parent, StringRef childName) {
    char path[PathMax];
    snprintf(path, PathMax, "%s/%s", GetEntityPath(parent), childName);

    return FindEntityByPath(path);
}

API_EXPORT StringRef GetEntityPath(Entity entity) {
    return GetNodeData(entity)->Path;
}


API_EXPORT StringRef GetEntityRelativePath(Entity entity, Entity relativeTo) {
    if(entity == relativeTo) return "";
    if(!IsEntityDecendant(entity, relativeTo)) return GetEntityPath(entity);

    StringRef parentPath = GetNodeData(relativeTo)->Path;

    return (StringRef)GetNodeData(entity)->Path + strlen(parentPath) + 1;
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

        char parentPath[PathMax];
        GetParentPath(parentPath, PathMax, path);

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
    SetParent(entity, parent);
    SetName(entity, name);
    return entity;
}

API_EXPORT bool IsEntityDecendant(Entity entity, Entity parent) {
        Assert(entity, IsEntityValid(entity));

        if(!HasComponent(entity, ComponentOf_Node()) || !IsEntityValid(parent)) {
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

    for(auto i = 0; i < GetComponentMax(ComponentOf_Component()); ++i) {
        auto component = GetComponentEntity(ComponentOf_Component(), i);

        if(HasComponent(templateEntity, component)) {
            for(auto j = 0; j < GetComponentMax(ComponentOf_Property()); ++j) {
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
            GetNodeData(parent)->FirstChild = GetNodeData(entity)->Sibling;
        } else {
            for(auto child = GetFirstChild(parent); child; child = GetSibling(child)) {
                auto sibling = GetSibling(child);
                if(sibling == entity) {
                    GetNodeData(child)->Sibling = GetNodeData(sibling)->Sibling;
                    break;
                }
            }
        }
    } else {
        if(FirstRoot == entity) {
            FirstRoot = GetNodeData(entity)->Sibling;
        } else {
            for(auto child = FirstRoot; child; child = GetSibling(child)) {
                auto sibling = GetSibling(child);
                if(sibling == entity) {
                    GetNodeData(child)->Sibling = GetNodeData(sibling)->Sibling;
                    break;
                }
            }
        }
    }

    auto data = GetNodeData(entity);
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
            GetNodeData(parent)->FirstChild = entity;
        } else {
            while(GetSibling(child)) {
                child = GetSibling(child);
            }

            Assert(entity, child != entity);
            GetNodeData(child)->Sibling = entity;
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
            GetNodeData(child)->Sibling = entity;
        }
    }

    auto data = GetNodeData(entity);
    CalculateEntityPath(data->Path, sizeof(data->Path), entity);

    EntityPathMap[data->Path] = entity;
    EntityMap[entity] = data->Path;
}

API_EXPORT void SetParent(Entity entity, Entity parent) {
    AddComponent(entity, ComponentOf_Node());
    if(IsEntityValid(parent)) AddComponent(parent, ComponentOf_Node());

    if(entity == parent) {
        Log(entity, LogSeverity_Error, "Cannot set parent to itself");
        return;
    }

    if(IsEntityValid(parent) && IsEntityDecendant(parent, entity)) {
        Log(entity, LogSeverity_Error, "Cannot attach itself to any entity in it's own subtree.");
        return;
    }

    auto data = GetNodeData(entity);
    Entity oldParent = data->Parent;

    if(IsEntityValid(parent) && oldParent != parent) {
        char newPath[PathMax];
        snprintf(newPath, PathMax, "%s/%s", GetEntityPath(parent), GetName(entity));

        auto existingChild = FindEntityByPath(newPath);
        if(IsEntityValid(existingChild)) {
            Log(entity, LogSeverity_Error, "Cannot attach '%s' to '%s'. An existing child with the same name already exist. Rename the child first to avoid two children with the same name.", GetEntityPath(entity), GetEntityPath(parent));
            return;
        }
    }

    if(oldParent != parent) {
        Detach(entity, oldParent);
        Attach(entity, parent);

        data->Parent = parent;

        const Type argumentTypes[] = {TypeOf_Entity, TypeOf_Entity, TypeOf_Entity};
        const void * argumentData[] = {&entity, &oldParent, &parent};

        auto changedEvent = GetPropertyChangedEvent(PropertyOf_Parent());
        FireEventFast(changedEvent, 3, argumentTypes, argumentData);
    }
}


API_EXPORT void SetName(Entity entity, StringRef name) {
    AddComponent(entity, ComponentOf_Node());

    auto data = GetNodeData(entity);
    if(strcmp(data->Name, name) != 0) {
        char newPath[sizeof(Node::Path)];
        char oldName[sizeof(Node::Name)];
        char oldPath[sizeof(Node::Path)];

        strncpy(oldName, data->Name, sizeof(data->Name));
        strncpy(oldPath, data->Path, sizeof(data->Path));

        strncpy(data->Name, name, sizeof(data->Name));

        CalculateEntityPath(newPath, sizeof(Node::Path), entity);
        auto existingEntity = FindEntityByPath(newPath);

        if(IsEntityValid(existingEntity)) {
            strncpy(data->Name, oldName, sizeof(data->Name));

            CalculateEntityPath(data->Path, sizeof(data->Path), entity);

            auto parentPath = IsEntityValid(data->Parent) ? GetEntityPath(data->Parent) : "<None>";
            Log(entity, LogSeverity_Error, "Cannot rename entity from '%s' to '%s' with parent '%s'. Sibling entity with the same entity already exists.", oldName, name, parentPath);
            return;
        }

        strncpy(data->Path, newPath, sizeof(data->Path));

        EntityPathMap.erase(oldPath);
        EntityMap.erase(entity);
        EntityPathMap[data->Path] = entity;
        EntityMap[entity] = data->Path;

        const Type argumentTypes[] = {TypeOf_Entity, TypeOf_StringRef, TypeOf_StringRef};
        const void * argumentData[] = {&entity, &oldName, &name};

        auto changedEvent = GetPropertyChangedEvent(PropertyOf_Name());
        FireEventFast(changedEvent, 3, argumentTypes, argumentData);
    }
}


LocalFunction(OnNodeRemoved, void, Entity component, Entity entity) {
    while(GetFirstChild(entity)) {
        DestroyEntity(GetFirstChild(entity));
    }

    Detach(entity, GetParent(entity));
}

LocalFunction(OnNodeAdded, void, Entity component, Entity entity) {
    Attach(entity, 0); // Attach to hierarchy as a root entity
}


BeginUnit(Node)
    BeginComponent(Node)
        RegisterProperty(Entity, Parent)
        RegisterProperty(StringRef, Name)
        RegisterProperty(bool, IsLocked)
    EndComponent()

    RegisterSubscription(EntityComponentAdded, OnNodeAdded, ComponentOf_Node())
    RegisterSubscription(EntityComponentRemoved, OnNodeRemoved, ComponentOf_Node())

    __ForceArgumentParse(EventOf_ParentChanged());
EndUnit()

void __InitializeNode() {
    auto component = ComponentOf_Node();
    __Property(PropertyOf_Name(), offsetof(Node, Name), sizeof(Node::Name), TypeOf_StringRef,  component);
    __Property(PropertyOf_Parent(), offsetof(Node, Parent), sizeof(Node::Parent), TypeOf_Entity,  component);
}

static void PrintNode(int level, Entity entity) {
    static const int identation = 4;

    printf("%*s - %s (i: %d) [", level * identation, " ", GetName(entity), GetEntityIndex(entity));
    for_entity(componentEntity, componentData, Component) {
        if(HasComponent(entity, componentEntity)) {
            printf("%s, ", GetName(componentEntity));
        }
    }
    printf("]\n");

    for_children(child, entity) {
        PrintNode(level + 1, child);
    }
}

API_EXPORT void DumpNode() {
    for(auto entity = GetNextEntity(0); IsEntityValid(entity); entity = GetNextEntity(entity)) {
        if(!HasComponent(entity, ComponentOf_Node()) || !IsEntityValid(GetParent(entity))) {
            PrintNode(0, entity);
        }
    }
}

API_EXPORT StringRef GetName(Entity entity)  {
    auto data = GetNodeData(entity);
    return data->Name;
}

API_EXPORT Entity GetParent(Entity entity)  {
    auto data = GetNodeData(entity);
    return data->Parent;
}