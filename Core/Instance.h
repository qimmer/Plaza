//
// Created by Kim on 27-09-2018.
//

#ifndef PLAZA_INSTANCE_H
#define PLAZA_INSTANCE_H

#include <Core/NativeUtils.h>

struct UnresolvedReference {
    Entity UnresolvedReferenceProperty;
    StringRef UnresolvedReferenceUuid;
    u32 UnresolvedReferenceArrayIndex;
};

struct UnresolvedEntity {
    ChildArray UnresolvedReferences;
};

struct Instance {
    Entity Prefab;
};

struct Prefab {

};

Unit(Instance)
    Component(UnresolvedReference)
        ReferenceProperty(Property, UnresolvedReferenceProperty)
        Property(StringRef, UnresolvedReferenceUuid)
        Property(u32, UnresolvedReferenceArrayIndex)

    Component(UnresolvedEntity)
        ArrayProperty(UnresolvedReference, UnresolvedReferences)

    Component(Instance)
        Property(Entity, Prefab)

    Component(Prefab)

    Function(ResolveReferences, void)

#endif //PLAZA_INSTANCE_H
