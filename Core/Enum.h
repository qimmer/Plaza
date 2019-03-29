//
// Created by Kim Johannsen on 20-03-2018.
//

#ifndef PLAZA_ENUM_H
#define PLAZA_ENUM_H

#include <Core/NativeUtils.h>

struct Enum {
    bool EnumCombinable;
    ChildArray EnumFlags;
};

struct EnumFlag {
    u64 EnumFlagValue;
};

Unit(Enum)
    Component(Enum)
        Property(bool, EnumCombinable)
        ArrayProperty(EnumFlag, EnumFlags)

    Component(EnumFlag)
        Property(u64, EnumFlagValue)

    Function(GetEnumName, StringRef, Entity e, u64 value)
    Function(GetEnumValue, u64, Entity e, StringRef name)

#endif //PLAZA_ENUM_H
