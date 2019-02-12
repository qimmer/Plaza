//
// Created by Kim on 03-01-2019.
//

#ifndef PLAZA_LOGICAL_H
#define PLAZA_LOGICAL_H

#include <Core/NativeUtils.h>

struct Switch {
    Variant SwitchInput;
    Variant SwitchResult;
};

struct SwitchCase {
    Variant SwitchCaseValue;
    Variant SwitchCaseResult;
};

Unit(Logical)
    Component(Switch)
        Property(Variant, SwitchInput)
        Property(Variant, SwitchResult)
        ArrayProperty(SwitchCase, SwitchCases)

    Component(SwitchCase)
        Property(Variant, SwitchCaseValue)
        Property(Variant, SwitchCaseResult)

    Component(ForEach)
        Property(Entity, ForEachSource)
        ReferenceProperty(Property, ForEachProperty)

    Component(FormatArgument)
        Property(Variant, FormatArgumentValue)

    Component(Format)
        Property(StringRef, FormatString)
        ArrayProperty(FormatArgument, FormatArguments)

#endif //PLAZA_LOGICAL_H
