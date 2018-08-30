//
// Created by Kim on 28-07-2018.
//

#ifndef PLAZA_ENTITYTRACKER_H
#define PLAZA_ENTITYTRACKER_H

#include <Core/NativeUtils.h>

Unit(EntityTracker)
    Component(EntityModification)
        Property(Entity, EntityModificationEntity)

    Component(EntityTracker)
        ArrayProperty(EntityModification, EntityModifications)

    Function(GetEntityTrackerChanges, u16, Entity tracker, Entity responseStream)

#endif //PLAZA_ENTITYTRACKER_H
