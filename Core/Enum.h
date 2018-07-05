//
// Created by Kim Johannsen on 20-03-2018.
//

#ifndef PLAZA_ENUM_H
#define PLAZA_ENUM_H

#include <Core/NativeUtils.h>

Unit(Enum)

Component(Enum)
    Property(bool, EnumCombinable)

Component(EnumFlag)
    Property(u32, EnumFlagValue)

#endif //PLAZA_ENUM_H
