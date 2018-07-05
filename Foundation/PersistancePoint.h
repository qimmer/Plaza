
#ifndef PersistancePoint_H
#define PersistancePoint_H

#include <Core/Entity.h>

DeclareComponent(PersistancePoint)
DeclareService(PersistancePoint)

DeclareComponentPropertyReactive(PersistancePoint, bool, Loaded)

typedef struct Serializer {
    EntityBoolHandler SerializeHandler;
    EntityBoolHandler DeserializeHandler;
} Serializer;

bool IsLoading(Entity persistancePoint);
void Load(Entity persistancePoint);
void Save(Entity persistancePoint);
void Unload(Entity persistancePoint);

DeclareEvent(LoadStarted, Entity entity)
DeclareEvent(SaveStarted, Entity entity)

DeclareEvent(UnloadFinished, Entity entity)
DeclareEvent(LoadFinished, Entity entity)
DeclareEvent(SaveFinished, Entity entity)

void AddSerializer(StringRef mimeType, struct Serializer* serializer);
void RemoveSerializer(StringRef mimeType);

#endif //PersistancePoint_H
