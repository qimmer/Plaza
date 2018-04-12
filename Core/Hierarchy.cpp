#include <Core/Hierarchy.h>
#include <Core/String.h>
#include <Core/Service.h>
#include <sstream>
#include <climits>
#include <Core/Dictionary.h>
#include <map>

    static Dictionary<Entity> EntityPathMap;
    std::map<Entity, String> EntityMap;

    static Entity FirstRoot = 0;

    struct Hierarchy {
        Entity Parent;
        String Name;
        Entity FirstChild;
        Entity Sibling;
        String Path;
    };

    DefineComponent(Hierarchy)
        DefinePropertyReactive(StringRef, Name)
        DefinePropertyReactive(Entity, Parent)
    EndComponent()

    DefineComponentPropertyReactive(Hierarchy, StringRef, Name)
    DefineComponentPropertyReactive(Hierarchy, Entity, Parent)

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

    Entity GetFirstChild(Entity parent) {
        // If parent is zero, return first root entity
        if(parent == 0) {
            return FirstRoot;
        }

        return GetHierarchy(parent)->FirstChild;
    }

    Entity GetSibling(Entity child) {
        if(!IsEntityValid(child)) return 0;

        return GetHierarchy(child)->Sibling;
    }

    Entity FindChild(Entity parent, StringRef childName) {
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", GetEntityPath(parent), childName);

        return FindEntity(path);
    }

    StringRef GetEntityPath(Entity entity) {
        return GetHierarchy(entity)->Path.c_str();
    }

    Entity CreateEntityFromPath(StringRef path) {
        Assert(path);
        if(path[0] != '/') {
            Log(LogChannel_Core, LogSeverity_Error, "Absolute entity path is malformed: %s", path);
            return 0;
        }

        auto existing = FindEntity(path);
        if(IsEntityValid(existing)) {
            return existing;
        }

        StringRef name = strrchr(path, '/');
        Assert(name);
		name++;

        char parentPath[PATH_MAX];
        GetParentPath(parentPath, PATH_MAX, path);

        auto parent = parentPath[0] ? CreateEntityFromPath(parentPath) : 0;

        return CreateEntityFromName(parent, name);
    }

Entity CreateEntityFromName(Entity parent, StringRef name) {
    Assert(name);

    auto nameLen = strlen(name);
    if(nameLen <= 0) {
        Log(LogChannel_Core, LogSeverity_Error, "Entity name cannot be empty");
        return 0;
    }

    for(auto i = 0; i < nameLen; ++i) {
        if(name[i] == ':' || name[i] == '/' || name[i] == '\\') {
            Log(LogChannel_Core, LogSeverity_Error, "Entity name '%s' contains invalid character: '%c'", name, name[i]);
            return 0;
        }
    }

    for_children(child, parent) {
        Assert(IsEntityValid(child));
        if(strcmp(GetName(child), name) == 0) {
            return child;
        }
    }

    auto entity = CreateEntity();
    AddHierarchy(entity);
    SetName(entity, name);
    SetParent(entity, parent);
    return entity;
}

    Entity FindEntity(StringRef path) {
        auto it = EntityPathMap.find(path);
        if(it != EntityPathMap.end()) {
            return it->second;
        }

        return 0;
    }

    bool IsEntityDecendant(Entity entity, Entity parent) {
        Assert(IsEntityValid(entity));

        if(!HasHierarchy(entity) || !IsEntityValid(parent)) {
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

Entity GetNextChild(Entity parent, Entity currentChild) {
    if(!currentChild) return GetFirstChild(parent);
    return GetSibling(currentChild);
}

Entity GetNextChildThat(Entity parent, Entity currentChild, EntityBoolHandler handler) {
    currentChild = GetNextChild(parent, currentChild);
    while(IsEntityValid(currentChild) && !handler(currentChild)) {
        currentChild = GetNextChild(parent, currentChild);
    }
    return currentChild;
}

Entity CopyEntity(Entity templateEntity, StringRef copyPath) {
    auto copy = CreateEntityFromPath(copyPath);

    for(auto property = GetNextProperty(0); property; property = GetNextProperty(property)) {
        auto componentType = GetPropertyOwner(property);
        if(componentType == TypeOf_Hierarchy()) continue;
        if(HasComponent(templateEntity, componentType)) {
            GetPropertyTransferFunc(property)(templateEntity, copy);
        }
    }

    return copy;
}

static void OnNameChanged(Entity entity, StringRef oldName, StringRef newName) {
    char newPath[PATH_MAX];
    CalculateEntityPath(newPath, PATH_MAX, entity);
    auto existingEntity = FindEntity(newPath);

    Assert(!IsEntityValid(existingEntity) || existingEntity == entity);

    auto data = GetHierarchy(entity);
    EntityPathMap.erase(data->Path);
    EntityMap.erase(entity);
    data->Path = newPath;
    EntityPathMap[data->Path] = entity;
    EntityMap[entity] = data->Path;
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

static void Attach(Entity entity, Entity parent) {
    if(IsEntityValid(parent)) {
        auto child = GetFirstChild(parent);
        if(!IsEntityValid(child)) {
            Assert(parent != entity);
            GetHierarchy(parent)->FirstChild = entity;
        } else {
            while(GetSibling(child)) {
                child = GetSibling(child);
            }

            Assert(child != entity);
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

            Assert(child != entity);
            GetHierarchy(child)->Sibling = entity;
        }
    }

    auto data = GetHierarchy(entity);
    char newPath[PATH_MAX];
    CalculateEntityPath(newPath, PATH_MAX, entity);
    data->Path = newPath;
    EntityPathMap[data->Path] = entity;
    EntityMap[entity] = data->Path;
}

static void OnParentChanged(Entity entity, Entity oldParent, Entity newParent) {
    if(oldParent == newParent) {
        return;
    }

    Detach(entity, oldParent);
    Attach(entity, newParent);
}

static void OnHierarchyRemoved(Entity entity) {
    while(GetFirstChild(entity)) {
        DestroyEntity(GetFirstChild(entity));
    }

    Detach(entity, GetParent(entity));
}

static void OnHierarchyAdded(Entity entity) {
    Attach(entity, 0); // Attach to hierarchy as a root entity
}

DefineService(Hierarchy)
        Subscribe(ParentChanged, OnParentChanged)
        Subscribe(NameChanged, OnNameChanged)
        Subscribe(HierarchyAdded, OnHierarchyAdded)
        Subscribe(HierarchyRemoved, OnHierarchyRemoved)
EndService()
