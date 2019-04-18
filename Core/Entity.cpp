#include <Core/Entity.h>
#include <Core/Pool.h>
#include <Core/Debug.h>
#include <Core/Function.h>

BeginUnit(Entity)
    RegisterFunction(IsEntityOccupied)
    RegisterFunction(IsEntityValid)
    RegisterFunction(CreateEntity)
    RegisterFunction(DestroyEntity)
EndUnit()

eastl::vector<u32> Generations;
eastl::vector<u32> FreeSlots;

extern bool __IsCoreInitialized;

#define Verbose_Entity "entity"

API_EXPORT u32 GetNumEntities() {
    return (u32)(Generations.size() - FreeSlots.size());
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

API_EXPORT Entity CreateEntity() {
    u32 index;
    if (FreeSlots.size()) {
        index = FreeSlots[FreeSlots.size() - 1];
        FreeSlots.pop_back();
    } else {
        Generations.push_back(0);
        index = (u32)(Generations.size() - 1);
    }
    
    Generations[index]++;
    
    Assert(0, Generations[index] % 2 != 0);
    
    auto entity = GetEntity(index, Generations[index]);
    auto argument = MakeVariant(Entity, entity);

    Verbose(Verbose_Entity, "Entity Created: %s", GetDebugName(entity));

    return entity;
}

API_EXPORT void DestroyEntity(Entity entity) {
    if (!IsEntityValid(entity)) {
        return;
    }

    for_entity(component, ComponentOf_Component()) {
        RemoveComponent(entity, component);
    }

    auto argument = MakeVariant(Entity, entity);
    auto index = GetEntityIndex(entity);

    // Only reuse entity when in release build. In debug, we want to preserve entity handle id's over time for easier debugging
#ifdef NDEBUG
    FreeSlots.push_back(index);
#endif

    ++ Generations[index];
    
    Assert(0, Generations[index] % 2 == 0);
    Verbose(Verbose_Entity, "Entity Destroyed: %s", GetDebugName(entity));
}
