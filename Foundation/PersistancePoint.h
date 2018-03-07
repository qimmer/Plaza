
#ifndef PersistancePoint_H
#define PersistancePoint_H

#include <Core/Entity.h>

DeclareComponent(PersistancePoint)
DeclareService(PersistancePoint)

DeclareComponentProperty(PersistancePoint, bool, Loaded)

typedef struct Serializer {
    EntityBoolHandler SerializeHandler;
    EntityBoolHandler DeserializeHandler;
} Serializer;

bool IsLoading(Entity persistancePoint);
void Load(Entity persistancePoint);
void Save(Entity persistancePoint);
void Unload(Entity persistancePoint);

DeclareEvent(LoadStarted, EntityHandler)
DeclareEvent(SaveStarted, EntityHandler)

DeclareEvent(UnloadFinished, EntityHandler)
DeclareEvent(LoadFinished, EntityHandler)
DeclareEvent(SaveFinished, EntityHandler)

void AddSerializer(StringRef mimeType, struct Serializer* serializer);
void RemoveSerializer(StringRef mimeType);

#endif //PersistancePoint_H
