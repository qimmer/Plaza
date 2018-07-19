#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <Core/NativeUtils.h>
#include <Core/Debug.h>

Unit(Node)
__Component(Node)
    Property(bool, IsLocked)
    Declare(Property, Parent)
    static StringRef __Parent__Meta = "";
    static StringRef __Name__Meta = "";
    Event(ParentChanged)
    Entity GetParent(Entity entity);
    void SetParent(Entity entity, Entity value);
    Declare(Property, Name, 0)
    struct NameChangedArgs { Entity ChangedEntity; StringRef OldValue; StringRef NewValue; };
    Event(NameChanged)
    StringRef GetName(Entity entity);
    void SetName(Entity entity, StringRef value);

Function(GetFirstChild, Entity, Entity parent)
Function(GetSibling, Entity, Entity child)

Function(IsEntityDecendant, bool, Entity child, Entity parent)

Function(GetEntityPath, StringRef, Entity entity)
Function(GetEntityRelativePath, StringRef, Entity entity, Entity relativeTo)

Function(CreateEntityFromPath, Entity, StringRef path)
Function(CreateEntityFromName, Entity, Entity parent, StringRef name)

Function(FindEntityByName, Entity, Entity component, StringRef name)
Function(FindEntityByPath, Entity, StringRef path)

#define for_children(VARNAME, PARENT) \
    for(auto VARNAME = GetFirstChild(PARENT); VARNAME; VARNAME = GetSibling(VARNAME))

void __InitializeNode();

void DumpNode();

#endif //HIERARCHY_H