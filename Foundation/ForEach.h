//
// Created by Kim on 25-02-2019.
//

#ifndef PLAZA_FOREACH_H
#define PLAZA_FOREACH_H

#include <Core/NativeUtils.h>

struct ForEach {
    Entity ForEachSourceEntity, ForEachDestinationEntity, ForEachSourceArrayProperty, ForEachDestinationArrayProperty, ForEachTemplate;
};

struct ForEachInstance {
    Entity ForEachModel;
};

Unit(ForEach)
    Component(ForEach)
        Property(Entity, ForEachSourceEntity)
        Property(Entity, ForEachDestinationEntity)
        ReferenceProperty(Property, ForEachSourceArrayProperty)
        ReferenceProperty(Property, ForEachDestinationArrayProperty)
        ChildProperty(Ownership, ForEachTemplate)

    Component(ForEachInstance)
        Property(Entity, ForEachModel)

#endif //PLAZA_FOREACH_H
