//
// Created by Kim on 21-11-2018.
//

#ifndef PLAZA_MATHOPERATION_H
#define PLAZA_MATHOPERATION_H

#include <Core/NativeUtils.h>

enum {
    MathOperation_Add,
    MathOperation_Sub,
    MathOperation_Mul,
    MathOperation_Div,
    MathOperation_Mod,
    MathOperation_Log,
    MathOperation_Pow,
    MathOperation_Cos,
    MathOperation_Sin,
    MathOperation_Tan,
    MathOperation_Atan,
    MathOperation_Max,
    MathOperation_Min
};

struct Math {
    Variant MathA, MathB, MathResult;
    u8 MathOperation;
};

struct VectorSplit {
    Variant VectorSplitValue;
    Variant VectorSplitX, VectorSplitY, VectorSplitZ, VectorSplitW;
};

Unit(MathOperation)
    Enum(MathOperation)
    Component(Math)
        Property(u8, MathOperation)
        Property(Variant, MathA)
        Property(Variant, MathB)
        Property(Variant, MathResult)
        
    Component(VectorSplit)
        Property(Variant, VectorSplitX)
        Property(Variant, VectorSplitY)
        Property(Variant, VectorSplitZ)
        Property(Variant, VectorSplitW)
        Property(Variant, VectorSplitValue)

    Function(Not, Variant, Variant value)
    Function(Equals, Variant, Variant value, Variant other)
    Function(Add, Variant, Variant value, Variant other)
    Function(Sub, Variant, Variant value, Variant other)
    Function(Mul, Variant, Variant value, Variant other)
    Function(Div, Variant, Variant value, Variant other)
    Function(Mod, Variant, Variant value, Variant other)
    Function(Sin, Variant, Variant value)
    Function(Cos, Variant, Variant value)
    Function(Tan, Variant, Variant value)
    Function(Pow, Variant, Variant value, Variant other)
    Function(Ratio, Variant, Variant value)
    Function(GetX, Variant, Variant vec)
    Function(GetY, Variant, Variant vec)
    Function(GetZ, Variant, Variant vec)
    Function(GetW, Variant, Variant vec)
    Function(Vec2, Variant, Variant x, Variant y)
    Function(Vec3, Variant, Variant x, Variant y)
    Function(Vec4, Variant, Variant x, Variant y)

#endif //PLAZA_MATHOPERATION_H
