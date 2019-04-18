//
// Created by Kim on 25-02-2019.
//

#ifndef PLAZA_SWITCH_H
#define PLAZA_SWITCH_H

#include <Core/NativeUtils.h>

struct Switch {
    Variant SwitchInput;
    Variant SwitchResult;
    ChildArray SwitchCases;
};

struct SwitchCase {
    Variant SwitchCaseValue;
    Variant SwitchCaseResult;
};

Unit(Switch)
    Component(Switch)
        Property(Variant, SwitchInput)
        Property(Variant, SwitchResult)
        ArrayProperty(SwitchCase, SwitchCases)

    Component(SwitchCase)
        Property(Variant, SwitchCaseValue)
        Property(Variant, SwitchCaseResult)

#endif //PLAZA_SWITCH_H
