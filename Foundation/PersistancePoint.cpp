
#include "PersistancePoint.h"
#include "Persistance.h"

struct PersistancePoint {
};

DefineComponent(PersistancePoint)
EndComponent()

DefineService(PersistancePoint)
EndService()

DefineEvent(LoadStarted, EntityHandler)
DefineEvent(SaveStarted, EntityHandler)
DefineEvent(LoadEnded, EntityHandler)
DefineEvent(SaveEnded, EntityHandler)

void Load(Entity persistancePoint) {
    FireEvent(LoadStarted, persistancePoint);
    FireEvent(LoadEnded, persistancePoint);
}

void Save(Entity persistancePoint) {
    FireEvent(SaveStarted, persistancePoint);
    FireEvent(SaveEnded, persistancePoint);
}

void Unload(Entity persistancePoint) {
    for(auto entity = GetNextEntityThat(0, HasPersistance); IsEntityValid(entity); entity = GetNextEntityThat(entity, HasPersistance)) {
        if(GetEntityPersistancePoint(entity) == persistancePoint) {
            DestroyEntity(entity);
        }
    }
}

static bool ServiceStart() {
    return true;
}

static bool ServiceStop() {
    return true;
}
