//
// Created by Kim Johannsen on 21/12/2017.
//

#include <Core/NativeUtils.h>
#include "Test.h"

struct Test {
    Vector(TestEntities, Entity, 128)
};

BeginUnit(Test)
    BeginComponent(Test)
        RegisterBase(Function)
        RegisterArrayProperty(Test, TestEntities)
    EndComponent()
EndUnit()