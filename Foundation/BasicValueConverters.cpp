//
// Created by Kim on 30-09-2018.
//

#include "BasicValueConverters.h"
#include <math.h>
#include <Core/Debug.h>

API_EXPORT Variant Not(Variant value) {
    value = Cast(value, TypeOf_bool);
    value.as_bool = !value.as_bool;
    return value;
}

API_EXPORT Variant Equals(Variant value, Variant other) {
    other = Cast(other, value.type);
    auto result = memcmp(&value.data, &other.data, GetTypeSize(value.type)) == 0;
    return MakeVariant(bool, result);
}

#define MATH_OP(NAME, OP) \
API_EXPORT Variant NAME(Variant value, Variant other) { \
    auto origType = value.type; \
    value = Cast(value, TypeOf_double); \
    other = Cast(other, TypeOf_double); \
\
    value.as_double OP other.as_double; \
\
    value = Cast(value, origType);\
    return value;\
}


#define MATH_FUNC(NAME, FUNC) \
API_EXPORT Variant NAME(Variant value) { \
    auto origType = value.type; \
    value = Cast(value, TypeOf_double); \
\
    value.as_double = FUNC(value.as_double); \
\
    value = Cast(value, origType);\
    return value;\
}

#define MATH_FUNC_2(NAME, FUNC) \
API_EXPORT Variant NAME(Variant value, Variant other) { \
    auto origType = value.type; \
    value = Cast(value, TypeOf_double); \
    other = Cast(other, TypeOf_double); \
\
    value.as_double = FUNC(value.as_double, other.as_double); \
\
    value = Cast(value, origType);\
    return value;\
}

MATH_OP(Add, +=)
MATH_OP(Sub, -=)
MATH_OP(Mul, *=)
MATH_OP(Div, /=)
MATH_FUNC(Sin, sin)
MATH_FUNC(Cos, cos)
MATH_FUNC(Tan, tan)
MATH_FUNC_2(Mod, fmod)
MATH_FUNC_2(Pow, pow)

API_EXPORT Variant Ratio(Variant value) {
    float ratio = 1.0f;
    switch(value.type) {
        case TypeOf_v2f:
            ratio = value.as_v2f.x / value.as_v2f.y;
            break;
        case TypeOf_v2i:
            ratio = float(value.as_v2i.x) / value.as_v2i.y;
            break;
        default:
            Log(0, LogSeverity_Error, "Ratio function only operates on v2* types. Type provided is: %s", GetTypeName(value.type));
            break;
    }

    Variant result = MakeVariant(float, ratio);

    return result;
}

BeginUnit(BasicValueConverters)
    RegisterFunction(Not)
    RegisterFunction(Equals)
    RegisterFunction(Add)
    RegisterFunction(Sub)
    RegisterFunction(Mul)
    RegisterFunction(Div)
    RegisterFunction(Mod)
    RegisterFunction(Sin)
    RegisterFunction(Cos)
    RegisterFunction(Tan)
    RegisterFunction(Pow)
    RegisterFunction(Ratio)
EndUnit()