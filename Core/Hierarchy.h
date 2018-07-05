#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <Core/NativeUtils.h>

Unit(Hierarchy)

Component(Hierarchy)
    Property(bool, IsLocked)
    Declare(Property, Parent, 0)
    struct ParentChangedArgs { Entity ChangedEntity; Entity OldValue; Entity NewValue; };
    Event(ParentChanged)
    inline Entity GetParent(Entity entity) {
        static Entity prop = PropertyOf_Parent();
        Entity value;
        memset(&value, 0, sizeof(Entity));
        GetPropertyValue(prop, entity, &value);
        return value;
    }
    void SetParent(Entity entity, Entity value);
    Declare(Property, Name, 0)
    struct NameChangedArgs { Entity ChangedEntity; StringRef OldValue; StringRef NewValue; };
    Event(NameChanged)
    inline StringRef GetName(Entity entity) {
        static Entity prop = PropertyOf_Name();
        StringRef value;
        memset(&value, 0, sizeof(StringRef));
        GetPropertyValue(prop, entity, &value);
        return value;
    }
    void SetName(Entity entity, StringRef value);

Function(GetFirstChild, Entity, Entity parent)
Function(GetSibling, Entity, Entity child)

Function(IsEntityDecendant, bool, Entity child, Entity parent)

Function(GetEntityPath, StringRef, Entity entity)

Function(CreateEntityFromPath, Entity, StringRef path)
Function(CreateEntityFromName, Entity, Entity parent, StringRef name)

Function(FindEntityByName, Entity, Entity component, StringRef name)
Function(FindEntityByPath, Entity, StringRef path)

#define for_children(VARNAME, PARENT) \
    for(auto VARNAME = GetFirstChild(PARENT); VARNAME; VARNAME = GetSibling(VARNAME))

void __InitializeHierarchy();

void DumpHierarchy();

#endif //HIERARCHY_H