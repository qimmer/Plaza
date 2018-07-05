#ifndef PLAZA_ENTITY_H
#define PLAZA_ENTITY_H

#include <Core/NativeUtils.h>

#define GetEntityIndex(handle) (((u32 *) &handle)[0])
#define GetEntityGeneration(handle) (((u32 *) &handle)[1])
#define GetEntity(index, generation) ((((Entity)generation) << 32) | (index))

Unit(Entity)

Function(CreateEntity, Entity)
Function(DestroyEntity, void, Entity entity)
Function(GetNextEntity, Entity, Entity previousEntity);
Function(IsEntityOccupied, bool, u32 index)
Function(IsEntityValid, bool, Entity handle)

struct EntityCreatedArgs { Entity entity; };
struct EntityDestroyedArgs { Entity entity; };

Event(EntityCreated)
Event(EntityDestroyed)

#endif
