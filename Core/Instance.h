//
// Created by Kim on 27-09-2018.
//

#ifndef PLAZA_INSTANCE_H
#define PLAZA_INSTANCE_H

#include <Core/NativeUtils.h>

struct UnresolvedReference {
    Entity UnresolvedReferenceEntity;
    Entity UnresolvedReferenceProperty;
    StringRef UnresolvedReferenceUuid;
};

struct UnresolvedEntity {
};

struct Template {};

struct Instance {
    bool InstanceIgnoreChanges;
    Entity InstanceTemplate;
};

struct InstanceOverride {
    Entity InstanceOverrideProperty;
};

Unit(Instance)
    Component(UnresolvedReference)
        ReferenceProperty(Property, UnresolvedReferenceProperty)
        Property(Entity, UnresolvedReferenceEntity)
        Property(StringRef, UnresolvedReferenceUuid)

    Component(UnresolvedEntity)
        ArrayProperty(UnresolvedReference, UnresolvedReferences)

    Component(Template)

    Component(InstanceOverride)
        ReferenceProperty(Property, InstanceOverrideProperty)

    Component(Instance)
        ReferenceProperty(Template, InstanceTemplate)
        ArrayProperty(InstanceOverride, InstanceOverrides)
        Property(bool, InstanceIgnoreChanges)

    Function(ResolveReferences, bool, Entity root)

#endif //PLAZA_INSTANCE_H
