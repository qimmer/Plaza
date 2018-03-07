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

DeclareComponentProperty(Hierarchy, StringRef, Name)
DeclareComponentProperty(Hierarchy, Entity, Parent)

Entity GetFirstChild(Entity parent);
Entity GetNextChild(Entity parent, Entity currentChild);
Entity GetNextChildThat(Entity parent, Entity currentChild, EntityBoolHandler handler);
Entity GetSibling(Entity child);

bool IsEntityDecendant(Entity entity, Entity parent);

StringRef GetEntityPath(Entity entity);
Entity CreateEntityFromPath(StringRef path);

Entity FindChild(Entity parent, StringRef childName);
Entity FindEntity(StringRef path);

#define DeclareComponentChild(TYPENAME, PROPERTYNAME) \
    Entity Get ## PROPERTYNAME (Entity entity);

#define DefineComponentChild(TYPENAME, CHILDCOMPONENTTYPE, PROPERTYNAME) \
    Entity Get ## PROPERTYNAME (Entity entity) { \
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        if(!IsEntityValid(Get ## TYPENAME (entity)->PROPERTYNAME)) { \
            Get ## TYPENAME (entity)->PROPERTYNAME = Create ## CHILDCOMPONENTTYPE (FormatString("%s/" #PROPERTYNAME, GetEntityPath(entity))); \
        }\
        return Get ## TYPENAME (entity)->PROPERTYNAME; \
    }


#endif //HIERARCHY_H