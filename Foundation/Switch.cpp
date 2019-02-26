//
// Created by Kim on 25-02-2019.
//

#include "Switch.h"

BeginUnit(Switch)
    BeginComponent(Switch)
        RegisterProperty(Variant, SwitchInput)
        RegisterProperty(Variant, SwitchResult)
        RegisterArrayProperty(SwitchCase, SwitchCases)
    EndComponent()

    BeginComponent(SwitchCase)
        RegisterProperty(Variant, SwitchCaseValue)
        RegisterProperty(Variant, SwitchCaseResult)
    EndComponent()
EndUnit()
