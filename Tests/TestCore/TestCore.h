//
// Created by Kim on 24/06/2018.
//

#ifndef PLAZA_TESTCORE_H
#define PLAZA_TESTCORE_H

#include <Core/NativeUtils.h>

Module(TestCore)

Unit(TestCore)

Component(TestCore)
    ChildProperty(Family, FamilyWithNoChildren)
    ChildProperty(Family, FamilyWithTwoChildren)
    ChildProperty(Family, FamilyInstance)

Component(Family)
    ArrayProperty(Person, FamilyMembers)

Component(Person)
    Property(u8, PersonAge)
    ReferenceProperty(Person, PersonPartner)
    ReferenceProperty(Person, PersonMother)
    ReferenceProperty(Person, PersonFather)

#endif //PLAZA_TESTCORE_H
