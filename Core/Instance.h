//
// Created by Kim on 27-09-2018.
//

#ifndef PLAZA_INSTANCE_H
#define PLAZA_INSTANCE_H

#include <Core/NativeUtils.h>

struct UnresolvedReference {
    Entity UnresolvedReferenceProperty;
    StringRef UnresolvedReferenceUuid;
};

struct UnresolvedEntity {
    ChildArray UnresolvedReferences;
};

struct Instance {
    Entity InstanceTemplate;
};

struct TemplatedComponent {
    Entity ComponentTemplate;
};

Unit(Instance)
    Component(UnresolvedReference)
        ReferenceProperty(Property, UnresolvedReferenceProperty)
        Property(StringRef, UnresolvedReferenceUuid)

    Component(UnresolvedEntity)
        ArrayProperty(UnresolvedReference, UnresolvedReferences)

    Component(Instance)
        Property(Entity, InstanceTemplate)

    Component(TemplatedComponent)
        ChildProperty(Identification, ComponentTemplate)

    Function(ResolveReferences, void)

#endif //PLAZA_INSTANCE_H
