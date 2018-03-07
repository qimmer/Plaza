//
// Created by Kim Johannsen on 25/01/2018.
//

#ifndef PLAZA_PERSISTANCE_H
#define PLAZA_PERSISTANCE_H

#include <Core/Entity.h>

DeclareComponent(Persistance)
DeclareService(Persistance)

DeclareComponentProperty(Persistance, Entity, EntityPersistancePoint)

#endif //PLAZA_PERSISTANCE_H
