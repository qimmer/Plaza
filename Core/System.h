//
// Created by Kim on 25-03-2019.
//

#ifndef PLAZA_SYSTEM_H
#define PLAZA_SYSTEM_H

#include <Core/NativeUtils.h>

struct System {
    Entity SystemUpdateFunction;
    float SystemOrder;
};

Unit(System)
    Component(System)
        ReferenceProperty(Component, SystemComponent)
        ReferenceProperty(Function, SystemUpdateFunction)

void UpdateSystems();
void UpdateSystem(Entity system);

#endif //PLAZA_SYSTEM_H
