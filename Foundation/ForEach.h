//
// Created by Kim on 25-02-2019.
//

#ifndef PLAZA_FOREACH_H
#define PLAZA_FOREACH_H

#include <Core/NativeUtils.h>

struct ForEach {
    Entity ForEachSourceEntity, ForEachDestinationEntity, ForEachSourceArrayProperty, ForEachDestinationArrayProperty, ForEachTemplate;
    bool ForEachEnabled;
};

struct ForEachInstance {
    Entity ForEachSource, ForEach;
};

Unit(ForEach)
    Component(ForEach)
        Property(bool, ForEachEnabled)
        Property(Entity, ForEachSourceEntity)
        Property(Entity, ForEachDestinationEntity)
        ReferenceProperty(Property, ForEachSourceArrayProperty)
        ReferenceProperty(Property, ForEachDestinationArrayProperty)
        Property(Entity, ForEachTemplate)

    Component(ForEachInstance)
        Property(Entity, ForEach)
        Property(Entity, ForEachSource)

#endif //PLAZA_FOREACH_H
