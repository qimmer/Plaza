//
// Created by Kim on 12-01-2019.
//

#ifndef PLAZA_CONSTANT_H
#define PLAZA_CONSTANT_H

#include <Core/NativeUtils.h>

struct Constant {
    Variant ConstantValue;
};

struct ConstantDictionary {};

Unit(Constant)
    Component(Constant)
        Property(Variant, ConstantValue)

    Component(ConstantDictionary)
        ArrayProperty(Constant, Constants)

#endif //PLAZA_CONSTANT_H
