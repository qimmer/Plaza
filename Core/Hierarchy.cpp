#include <Core/Hierarchy.h>
#include <Core/String.h>
#include <Core/Service.h>
#include <sstream>
#include <climits>
#include <Core/Dictionary.h>
#include <map>

    static Dictionary<String, Entity> EntityPathMap;
    std::map<Entity, String> EntityMap;

    struct Hierarchy {
        Hierarchy() : Parent(0), FirstChild(0), Sibling(0) {}

        Entity Parent;
        String Name;
        Entity FirstChild;
        Entity Sibling;
        String Path;
    };

    DefineComponent(Hierarchy)
        DefineProperty(StringRef, Name)
        DefineProperty(Entity, Parent)
    EndComponent()

    DefineService(Hierarchy)
    EndService()

    DefineComponentProperty(Hierarchy, StringRef, Name)

    DefineComponentProperty(Hierarchy, Entity, Parent)

    static StringRef CalculateEntityPath(Entity entity) {
        static char path[PATH_MAX];
        static char path2[PATH_MAX];
        sprintf(path, "");
        sprintf(path2, "");

        while(IsEntityValid(entity)) {
            sprintf(path2, "/%s%s", GetName(entity), path);
            entity = GetParent(entity);
            strcpy(path, path2);
        }

        return path2;
    }

    static void GetParentPath(char *dest, u32 maxSize, StringRef path) {
        char *lastSep;
        strcpy(dest, path);
        lastSep = strrchr(dest, '/');
        if(lastSep != NULL) {
            *lastSep = '\0';
        }
    }

    Entity GetFirstChild(Entity parent) {
        // If parent is zero, return first root entity
        if(parent == 0) {
            auto entity = GetNextEntity(0);
            while(IsEntityValid(entity) && (!HasHierarchy(entity) || IsEntityValid(GetParent(entity)))) {
                entity = GetNextEntity(entity);
            }
            return entity;
        }

        if(!HasHierarchy(parent)) return 0;

        return GetHierarchy(parent)->FirstChild;
    }

    Entity GetSibling(Entity child) {
        if(!HasHierarchy(child)) return 0;

        // If we have no parent, return next root entity
        if(!HasHierarchy(child) || !GetParent(child)) {
            auto entity = GetNextEntity(child);
            while(IsEntityValid(entity) && (!HasHierarchy(entity) || IsEntityValid(GetParent(entity)))) {
                entity = GetNextEntity(entity);
            }
            return entity;
        }

        return GetHierarchy(child)->Sibling;
    }

    Entity FindChild(Entity parent, StringRef childName) {
        char path[PATH_MAX];
        sprintf(path, "%s/%s", GetEntityPath(parent), childName);

        return FindEntity(path);
    }

    StringRef GetEntityPath(Entity entity) {
        if(!IsEntityValid(entity) || !HasHierarchy(entity)) {
            return "";
        }

        return GetHierarchy(entity)->Path.c_str();
    }

    Entity CreateEntityFromPath(StringRef path) {
        if(path[0] != '/') {
            Log(LogChannel_Core, LogSeverity_Error, "Absolute entity path is malformed: %s", path);
            return 0;
        }

        auto existing = FindEntity(path);
        if(IsEntityValid(existing)) {
            return existing;
        }

        StringRef name = strrchr(path, '/') + 1;
        Assert(name);

        char parentPath[PATH_MAX];
        GetParentPath(parentPath, PATH_MAX, path);

        auto parent = parentPath[0] ? CreateEntityFromPath(parentPath) : 0;

        auto entity = CreateEntity();
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
    if(!HasHierarchy(parent) || !IsEntityValid(GetFirstChild(parent))) return 0;
    if(!IsEntityValid(currentChild)) return GetFirstChild(parent);
    return GetSibling(currentChild);
}

Entity GetNextChildThat(Entity parent, Entity currentChild, EntityBoolHandler handler) {
    currentChild = GetNextChild(parent, currentChild);
    while(IsEntityValid(currentChild) && !handler(currentChild)) {
        currentChild = GetNextChild(parent, currentChild);
    }
    return currentChild;
}

static void OnNameChanged(Entity entity, StringRef oldName, StringRef newName) {
        auto newPath = CalculateEntityPath(entity);
        auto existingEntity = FindEntity(newPath);
        Assert(!IsEntityValid(existingEntity) || existingEntity == entity);

        auto data = GetHierarchy(entity);
        EntityPathMap.erase(data->Path);
        EntityMap.erase(entity);
        data->Path = newPath;
        EntityPathMap[data->Path] = entity;
        EntityMap[entity] = data->Path;
    }

    static void OnParentChanged(Entity entity, Entity oldParent, Entity newParent) {
        if(oldParent == newParent) {
            return;
        }

        auto newPath = CalculateEntityPath(entity);
        auto existingEntity = FindEntity(newPath);
        Assert(!IsEntityValid(existingEntity));

        // Remove entity from old parent child list
        if(IsEntityValid(oldParent)) {
            if(GetFirstChild(oldParent) == entity) {
                GetHierarchy(oldParent)->FirstChild = GetHierarchy(entity)->Sibling;
            } else {
                for(auto child = GetFirstChild(oldParent); IsEntityValid(child); child = GetSibling(child)) {
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

        // Add entity to end of new parent child list
        if(IsEntityValid(newParent)) {
            if(!HasHierarchy(newParent)) AddHierarchy(newParent);

            auto child = GetFirstChild(newParent);
            if(!IsEntityValid(child)) {
                GetHierarchy(newParent)->FirstChild = entity;
            } else {
                while(IsEntityValid(GetSibling(child))) {
                    child = GetSibling(child);
                }

                GetHierarchy(child)->Sibling = entity;
            }
        }

        data->Path = newPath;
        EntityPathMap[data->Path] = entity;
        EntityMap[entity] = data->Path;
    }

    static void OnHierarchyRemoved(Entity entity) {
        while(GetFirstChild(entity)) {
            DestroyEntity(GetFirstChild(entity));
        }

        SetParent(entity, 0);

        EntityPathMap.erase(GetHierarchy(entity)->Path);
    }

    static void OnHierarchyAdded(Entity entity) {
        char name[32];
        sprintf(name, "Entity_%d", GetHandleIndex(entity));

        SetName(entity, name);
    }

    static bool ServiceStart() {
        SubscribeParentChanged(OnParentChanged);
        SubscribeNameChanged(OnNameChanged);
        SubscribeHierarchyAdded(OnHierarchyAdded);
        SubscribeHierarchyRemoved(OnHierarchyRemoved);

        return true;
    }

    static bool ServiceStop() {
        UnsubscribeParentChanged(OnParentChanged);
        UnsubscribeNameChanged(OnNameChanged);
        UnsubscribeHierarchyAdded(OnHierarchyAdded);
        UnsubscribeHierarchyRemoved(OnHierarchyRemoved);

        return true;
    }

