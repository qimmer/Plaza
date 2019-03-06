#ifndef PLAZA_ENTITY_H
#define PLAZA_ENTITY_H

#include <Core/NativeUtils.h>

#define GetEntityIndex(handle) (((u32 *) &handle)[0])
#define GetEntityGeneration(handle) (((u32 *) &handle)[1])
#define GetEntity(index, generation) ((((Entity)generation) << 32) | (index))

Function(CreateEntity, Entity);
Function(DestroyEntity, void, Entity entity)
Entity GetNextEntity(Entity previousEntity);

Unit(Entity)
    Function(IsEntityOccupied, bool, u32 index)
    Function(IsEntityValid, bool, Entity handle)
    Function(GetEntityByIndex, Entity, u32 index)
    Function(GetNumEntities, u32)
    static const StringRef __EventEntityCreated = "";
    inline Entity EventOf_EntityCreated() {\
        static Entity entity = 0;
        if(entity == 0) {
            entity = GetUniqueEntity("Event EntityCreated", NULL);
        }
        return entity;
    }

	inline bool IsEntityValid(Entity entity) {
		//auto index = GetEntityIndex(entity);
		return entity;// && Generations.size() > index && Generations[index] == GetEntityGeneration(entity);
	}

    Event(EntityDestroyed)

#endif
