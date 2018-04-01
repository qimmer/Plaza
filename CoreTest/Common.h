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
DeclareComponentProperty(Person, StringRef, PersonName)

DefineComponent(Person)
    DefinePropertyReactive(Person, PersonName)
EndComponent()

DefineComponentProperty(Person, StringRef, PersonName)


#endif //PLAZA_CORETEST_COMMON_H
