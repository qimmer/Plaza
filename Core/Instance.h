//
// Created by Kim on 27-09-2018.
//

#ifndef PLAZA_INSTANCE_H
#define PLAZA_INSTANCE_H

#include <Core/NativeUtils.h>

Unit(Instance)
    Component(Template)

    Component(InstanceOverride)
        ReferenceProperty(Property, InstanceOverrideProperty)

    Component(Instance)
        ReferenceProperty(Template, InstanceTemplate)
        ArrayProperty(InstanceOverride, InstanceOverrides)
        Property(bool, InstanceIgnoreChanges)

Function(Instantiate, void, Entity templateEntity, Entity destinationEntity)

#endif //PLAZA_INSTANCE_H
