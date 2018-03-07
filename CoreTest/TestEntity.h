//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_TESTENTITY_H
#define PLAZA_TESTENTITY_H

#include <Test/Test.h>
#include <Core/Entity.h>
#include <Core/Module.h>
#include "Common.h"

TestResult TestEntity()
{
    Entity entities[] =
            {
                    CreateEntity(),
                    CreateEntity(),
                    CreateEntity(),
                    CreateEntity(),
                    0
            };

    for(auto i = 0; i < 4; ++i)
    {
        Verify("Entity should be valid", IsEntityValid(entities[i]));
    }

    DestroyEntity(entities[0]);
    Verify("Entity should be invalid", !IsEntityValid(entities[0]));

    for(auto i = 0; entities[i]; ++i)
    {
        DestroyEntity(entities[i]);
    }

    return Success;
}

TestResult TestComponent() {
    auto entity = CreateEntity();

    Assert(!HasPerson(entity));
    Assert(AddPerson(entity));
    Assert(HasPerson(entity));
    SetPersonName(entity, "John");

    Assert(strcmp(GetPersonName(entity), "John") == 0);

    Assert(RemovePerson(entity));
    Assert(!RemovePerson(entity));
    Assert(AddPerson(entity));
    Assert(!AddPerson(entity));

    Assert(HasPerson(entity));

    Assert(RemovePerson(entity));
    Assert(!HasPerson(entity));

    DestroyEntity(entity);

    return Success;

}

#endif //PLAZA_TESTENTITY_H
