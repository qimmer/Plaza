#include <Core/Entity.h>
#include <Core/Pool.h>
#include <Core/Debug.h>
#include <Core/Function.h>
#include "Event.h"

BeginUnit(Entity)
    RegisterFunction(IsEntityOccupied)
    RegisterFunction(IsEntityValid)

    RegisterEvent(EntityCreated)
    RegisterEvent(EntityDestroyed)
EndUnit()

Vector<u32> Generations;
Vector<u32> FreeSlots;

extern bool __IsCoreInitialized;

API_EXPORT u32 GetNumEntities() {
    return Generations.size() - FreeSlots.size();
}

API_EXPORT bool IsEntityOccupied(u32 index) {
    return Generations.size() > index && Generations[index] % 2 != 0;
}

API_EXPORT bool IsEntityValid(Entity entity) {
    auto index = GetEntityIndex(entity);
    return entity && Generations.size() > index && Generations[index] == GetEntityGeneration(entity);
}

API_EXPORT Entity GetEntityByIndex (u32 index) {
    if(index >= Generations.size()) return 0;
    if(Generations[index] % 2 == 0) return 0;
    return GetEntity(index, Generations[index]);
}

API_EXPORT Entity GetNextEntity (Entity entity) {
    auto index = GetEntityIndex(entity); 
    if(index >= Generations.size()) return 0; 
    if(!entity) {
        while(!IsEntityOccupied(index)) {
            index++;
            if(index >= Generations.size()) return 0; 
        }
        return GetEntity(index, Generations[index]); 
    }
    index++;
    if(index >= Generations.size()) return 0; 
    while(!IsEntityOccupied (index)) { 
        if(index >= Generations.size()) return 0; 
        index++; 
    } 
    auto gen = Generations[index]; 
    return GetEntity(index, gen); 
} 

API_EXPORT Entity __CreateEntity () {
    u32 index;
    if (FreeSlots.size()) {
        index = FreeSlots[FreeSlots.size() - 1];
        FreeSlots.pop_back();
    } else {
        Generations.push_back(0);
        index = Generations.size() - 1;
    }
    
    Generations[index]++;
    
    Assert(0, Generations[index] % 2 != 0);
    
    auto entity = GetEntity(index, Generations[index]);

    Verbose(VerboseLevel_ComponentEntityCreationDeletion, "Entity Created: %s", GetDebugName(entity));

    if(__IsCoreInitialized) {
		Type types[] = { TypeOf_Entity };
		const void* values[] = { &entity };
        FireEventFast(EventOf_EntityCreated(), 1, types, values);
    }

    return entity;
}

void __DestroyEntity(Entity entity) {
    if (!IsEntityValid(entity)) {
        return;
    }

    for_entity(component, componentData, Component) {
        RemoveComponent(entity, component);
    }

	Type types[] = { TypeOf_Entity };
	const void* values[] = { &entity };
	FireEventFast(EventOf_EntityDestroyed(), 1, types, values);
    
    auto index = GetEntityIndex(entity);
    FreeSlots.push_back(index);
    ++ Generations[index];
    
    Assert(0, Generations[index] % 2 == 0);
    Verbose(VerboseLevel_ComponentEntityCreationDeletion, "Entity Destroyed: %s", GetDebugName(entity));
}
