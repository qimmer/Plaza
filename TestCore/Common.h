//
// Created by Kim Johannsen on 03/01/2018.
//

#ifndef PLAZA_CORETEST_COMMON_H
#define PLAZA_CORETEST_COMMON_H

#include <Core/String.h>
#include <Core/Entity.h>
#include <Core/Vector.h>
#include <Core/Pool.h>


    struct Person
    {
        String PersonName;
    };

    DeclareComponent(Person)

    void SetPersonName(Entity entity, StringRef name);
    StringRef GetPersonName(Entity entity);

    DefineComponent(Person)
        DefineProperty(Person, PersonName)
    EndComponent()

    void SetPersonName(Entity entity, StringRef name)
    {
        GetPerson(entity)->PersonName = name;
    }

    StringRef GetPersonName(Entity entity)
    {
        return GetPerson(entity)->PersonName.c_str();
    }

}

#endif //PLAZA_CORETEST_COMMON_H
