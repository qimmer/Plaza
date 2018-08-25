//
// Created by Kim Johannsen on 20-03-2018.
//

#ifndef PLAZA_ENUM_H
#define PLAZA_ENUM_H

#include <Core/NativeUtils.h>

Unit(Enum)
    Component(Enum)
        Property(bool, EnumCombinable)
        ArrayProperty(EnumFlag, EnumFlags)

    Component(EnumFlag)
        Property(u64, EnumFlagValue)

    Function(GetEnumName, StringRef, Entity e, u64 value)

#endif //PLAZA_ENUM_H
