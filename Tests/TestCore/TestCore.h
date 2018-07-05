//
// Created by Kim on 24/06/2018.
//

#ifndef PLAZA_TESTCORE_H
#define PLAZA_TESTCORE_H

#include <Core/NativeUtils.h>

Module(TestCore)

Unit(TestCore)

Component(Person)
    Property(u8, PersonAge)
    Property(StringRef, PersonFirstName)

#endif //PLAZA_TESTCORE_H
