//
// Created by Kim Johannsen on 21/12/2017.
//

#include <Core/NativeUtils.h>
#include "Test.h"

struct Test {

};

BeginUnit(Test)
    BeginComponent(Test)
        RegisterBase(Function)
    EndComponent()
EndUnit()