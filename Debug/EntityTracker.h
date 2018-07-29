//
// Created by Kim on 28-07-2018.
//

#ifndef PLAZA_ENTITYTRACKER_H
#define PLAZA_ENTITYTRACKER_H

#include <Core/NativeUtils.h>

#define EntityModificationType_Add 1
#define EntityModificationType_Remove 2

Unit(EntityTracker)

    Component(EntityModification)
        Property(Entity, EntityModificationEntity)

    Component(EntityTracker)
        ArrayProperty(EntityModification, EntityModifications)

#endif //PLAZA_ENTITYTRACKER_H
