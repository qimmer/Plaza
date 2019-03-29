//
// Created by Kim on 25-03-2019.
//

#ifndef PLAZA_SYSTEM_H
#define PLAZA_SYSTEM_H

#include <Core/NativeUtils.h>
#include <EASTL/map.h>
#include <EASTL/unordered_map.h>

struct System {
    Entity SystemComponent;
    NativePtr SystemFunction;
    float SystemOrder;
    bool SystemDeferred;
};

Unit(System)
    Component(System)
        Property(Entity, SystemComponent)
        Property(NativePtr, SystemFunction)
        Property(float, SystemOrder)
        Property(bool, SystemDeferred)

void NotifyChange(Entity entity, Entity component, const void *oldData, const void *newData);
void NotifyArray(Entity entity, Entity property, Entity oldValue, Entity newValue);
void ProcessSystems();

#endif //PLAZA_SYSTEM_H
