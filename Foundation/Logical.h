//
// Created by Kim on 03-01-2019.
//

#ifndef PLAZA_LOGICAL_H
#define PLAZA_LOGICAL_H

#include <Core/NativeUtils.h>

struct Format {
    StringRef FormatString;
    ChildArray FormatArguments;
};

struct FormatArgument {
    Variant FormatArgumentValue;
};

Unit(Logical)
    Component(FormatArgument)
        Property(Variant, FormatArgumentValue)

    Component(Format)
        Property(StringRef, FormatString)
        ArrayProperty(FormatArgument, FormatArguments)

#endif //PLAZA_LOGICAL_H
