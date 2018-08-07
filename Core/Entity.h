#ifndef PLAZA_ENTITY_H
#define PLAZA_ENTITY_H

#include <Core/NativeUtils.h>

#define GetEntityIndex(handle) (((u32 *) &handle)[0])
#define GetEntityGeneration(handle) (((u32 *) &handle)[1])
#define GetEntity(index, generation) ((((Entity)generation) << 32) | (index))

Entity __CreateEntity();
void __DestroyEntity(Entity entity);
Entity GetNextEntity(Entity previousEntity);

Unit(Entity)
    Function(IsEntityOccupied, bool, u32 index)
    Function(IsEntityValid, bool, Entity handle)

    static const StringRef __EventEntityCreated = "";
    inline Entity EventOf_EntityCreated() {\
        static Entity entity = 0;
        if(entity == 0) {
            entity = GetUniqueEntity("Event EntityCreated", NULL);
        }
        return entity;
    }

    Event(EntityDestroyed)

#endif
