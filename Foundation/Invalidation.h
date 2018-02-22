//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_INVALIDATION_H
#define PLAZA_INVALIDATION_H

#include <Core/Entity.h>
#include <Core/Service.h>


    DeclareComponent(Invalidation)
    DeclareService(Invalidation)

    DeclareComponentProperty(Invalidation, bool, Invalidated)

    void ValidateAll(EntityHandler validator, EntityBoolHandler condition);


#endif //PLAZA_INVALIDATION_H
