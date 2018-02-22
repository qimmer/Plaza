
#ifndef PersistancePoint_H
#define PersistancePoint_H

#include <Core/Entity.h>

DeclareComponent(PersistancePoint)
DeclareService(PersistancePoint)

void Load(Entity persistancePoint);
void Save(Entity persistancePoint);
void Unload(Entity persistancePoint);

DeclareEvent(LoadStarted, EntityHandler)
DeclareEvent(SaveStarted, EntityHandler)

DeclareEvent(UnloadFinished, EntityHandler)
DeclareEvent(LoadFinished, EntityHandler)
DeclareEvent(SaveFinished, EntityHandler)

#endif //PersistancePoint_H
