//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_TESTTYPE_H
#define PLAZA_TESTTYPE_H

#include <Test/Test.h>
#include <Core/Type.h>
#include <Core/Property.h>
#include "Common.h"

TestResult TestType()
{
    auto entity = CreateEntity();

    DestroyEntity(entity);

    return Success;
}

#endif //PLAZA_TESTTYPE_H
