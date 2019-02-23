//
// Created by Kim on 12-01-2019.
//

#include "Constant.h"

BeginUnit(Constant)
    BeginComponent(Constant)
        RegisterProperty(Variant, ConstantValue)
    EndComponent()
    BeginComponent(ConstantDictionary)
        RegisterArrayProperty(Constant, Constants)
    EndComponent()
EndUnit()