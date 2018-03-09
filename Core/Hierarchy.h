#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <Core/Entity.h>
#include <Core/Service.h>

DeclareComponent(Hierarchy)
DeclareService(Hierarchy)

typedef void(*ParentChangedHandler)(Entity entity, Entity oldParent, Entity newParent);
typedef void(*NameChangedHandler)(Entity entity, StringRef oldName, StringRef newName);

DeclareEvent(ParentChanged, ParentChangedHandler)
DeclareEvent(NameChanged, NameChangedHandler)

DeclareComponentPropertyReactive(Hierarchy, StringRef, Name)
DeclareComponentPropertyReactive(Hierarchy, Entity, Parent)

Entity GetFirstChild(Entity parent);
Entity GetNextChild(Entity parent, Entity currentChild);
Entity GetNextChildThat(Entity parent, Entity currentChild, EntityBoolHandler handler);
Entity GetSibling(Entity child);

bool IsEntityDecendant(Entity entity, Entity parent);

StringRef GetEntityPath(Entity entity);
Entity CreateEntityFromPath(StringRef path);

Entity FindChild(Entity parent, StringRef childName);
Entity FindEntity(StringRef path);

Entity CopyEntity(Entity templateEntity, StringRef copyPath);

#define DeclareComponentChild(TYPENAME, PROPERTYNAME) \
    Entity Get ## PROPERTYNAME (Entity entity);

#define DefineComponentChild(TYPENAME, CHILDCOMPONENTTYPE, PROPERTYNAME) \
    Entity Get ## PROPERTYNAME (Entity entity) { \
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        if(!IsEntityValid(Get ## TYPENAME (entity)->PROPERTYNAME)) { \
            char path[PATH_MAX];\
            snprintf(path, PATH_MAX, "%s/" #PROPERTYNAME, GetEntityPath(entity));\
            Get ## TYPENAME (entity)->PROPERTYNAME = Create ## CHILDCOMPONENTTYPE (path); \
        }\
        return Get ## TYPENAME (entity)->PROPERTYNAME; \
    }

#define for_children(VARNAME, PARENT) \
    for(auto VARNAME = GetFirstChild(PARENT); VARNAME; VARNAME = GetSibling(VARNAME))


#endif //HIERARCHY_H