//
// Created by Kim on 21-11-2018.
//

#include "MathOperation.h"


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
    value = Cast(value, TypeOf_v4f); \
    other = Cast(other, TypeOf_v4f); \
\
    value.as_v4f.x OP other.as_v4f.x; \
    value.as_v4f.y OP other.as_v4f.y; \
    value.as_v4f.z OP other.as_v4f.z; \
    value.as_v4f.w OP other.as_v4f.w; \
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
            Error(0, "Ratio function only operates on v2* types. Type provided is: %s", GetTypeName(value.type));
            break;
    }

    Variant result = MakeVariant(float, ratio);

    return result;
}

static Variant Evaluate(Variant a, Variant b, u8 op) {
    switch(op) {
        case MathOperation_Add:
            return Add(a, b);
        case MathOperation_Sub:
            return Sub(a, b);
        case MathOperation_Mul:
            return Mul(a, b);
        case MathOperation_Div:
            return Div(a, b);
        case MathOperation_Mod:
            return Mod(a, b);
        case MathOperation_Pow:
            return Pow(a, b);
        case MathOperation_Cos:
            return Cos(a);
        case MathOperation_Sin:
            return Sin(a);
        case MathOperation_Tan:
            return Tan(a);
    }

    return Variant_Empty;
}

static void OnMathChanged(Entity math, const Math& oldData, const Math& newData) {
    auto data = newData;
    if(data.MathA.type && data.MathB.type) {
        auto newResult = Evaluate(data.MathA, data.MathB, data.MathOperation);
        if(memcmp(&newResult, &data.MathResult, sizeof(Variant)) != 0) {
            data.MathResult = newResult;
            SetMath(math, data);
        }
    }
}

static void OnVectorSplitChanged(Entity split, const VectorSplit& oldData, const VectorSplit& newData) {
    auto data = newData;

    if(memcmp(&oldData.VectorSplitValue, &newData.VectorSplitValue, sizeof(Variant)) == 0) {
        // Vector has changed

        if(data.VectorSplitW.type) {
            switch(data.VectorSplitX.type) {
                case TypeOf_s32:
                {
                    v4i value = {
                            Cast(data.VectorSplitX, TypeOf_s32).as_s32,
                            Cast(data.VectorSplitY, TypeOf_s32).as_s32,
                            Cast(data.VectorSplitZ, TypeOf_s32).as_s32,
                            Cast(data.VectorSplitW, TypeOf_s32).as_s32
                    };

                    data.VectorSplitValue = MakeVariant(v4i, value);
                }
                case TypeOf_float:
                {
                    v4f value = {
                            Cast(data.VectorSplitX, TypeOf_float).as_float,
                            Cast(data.VectorSplitY, TypeOf_float).as_float,
                            Cast(data.VectorSplitZ, TypeOf_float).as_float,
                            Cast(data.VectorSplitW, TypeOf_float).as_float
                    };
                    data.VectorSplitValue = MakeVariant(v4f, value);
                }
            }
        }
        if(data.VectorSplitZ.type) {
            switch(data.VectorSplitX.type) {
                case TypeOf_s32:
                {
                    v3i value = {
                            Cast(data.VectorSplitX, TypeOf_s32).as_s32,
                            Cast(data.VectorSplitY, TypeOf_s32).as_s32,
                            Cast(data.VectorSplitZ, TypeOf_s32).as_s32
                    };
                    data.VectorSplitValue = MakeVariant(v3i, value);
                }
                case TypeOf_float:
                {
                    v3f value = {
                            Cast(data.VectorSplitX, TypeOf_float).as_float,
                            Cast(data.VectorSplitY, TypeOf_float).as_float,
                            Cast(data.VectorSplitZ, TypeOf_float).as_float
                    };
                    data.VectorSplitValue = MakeVariant(v3f, value);
                }
            }
        }
        if(data.VectorSplitY.type) {
            switch(data.VectorSplitX.type) {
                case TypeOf_s32:
                {
                    v2i value = {
                            Cast(data.VectorSplitX, TypeOf_s32).as_s32,
                            Cast(data.VectorSplitY, TypeOf_s32).as_s32
                    };
                    data.VectorSplitValue = MakeVariant(v2i, value);
                }
                case TypeOf_float:
                {
                    v2f value = {
                            Cast(data.VectorSplitX, TypeOf_float).as_float,
                            Cast(data.VectorSplitY, TypeOf_float).as_float
                    };
                    data.VectorSplitValue = MakeVariant(v2f, value);
                }
            }
        }

        if(memcmp(&data.VectorSplitValue, &oldData.VectorSplitValue, sizeof(Variant)) != 0) {
            SetVectorSplit(split, data);
        }
    } else {
        // Value has changed

        switch(newData.VectorSplitValue.type) {
            case TypeOf_v2i:
                data.VectorSplitX = MakeVariant(s32, newData.VectorSplitValue.as_v2i.x);
                data.VectorSplitY = MakeVariant(s32, newData.VectorSplitValue.as_v2i.y);
                data.VectorSplitZ = Variant_Empty;
                data.VectorSplitW = Variant_Empty;
                break;
            case TypeOf_v3i:
                data.VectorSplitX = MakeVariant(s32, newData.VectorSplitValue.as_v3i.x);
                data.VectorSplitY = MakeVariant(s32, newData.VectorSplitValue.as_v3i.y);
                data.VectorSplitZ = MakeVariant(s32, newData.VectorSplitValue.as_v3i.z);
                data.VectorSplitW = Variant_Empty;
                break;
            case TypeOf_v4i:
                data.VectorSplitX = MakeVariant(s32, newData.VectorSplitValue.as_v4i.x);
                data.VectorSplitY = MakeVariant(s32, newData.VectorSplitValue.as_v4i.y);
                data.VectorSplitZ = MakeVariant(s32, newData.VectorSplitValue.as_v4i.z);
                data.VectorSplitW = MakeVariant(s32, newData.VectorSplitValue.as_v4i.w);
                break;
            case TypeOf_v2f:
                data.VectorSplitX = MakeVariant(float, newData.VectorSplitValue.as_v2f.x);
                data.VectorSplitY = MakeVariant(float, newData.VectorSplitValue.as_v2f.y);
                data.VectorSplitZ = Variant_Empty;
                data.VectorSplitW = Variant_Empty;
                break;
            case TypeOf_v3f:
                data.VectorSplitX = MakeVariant(float, newData.VectorSplitValue.as_v3f.x);
                data.VectorSplitY = MakeVariant(float, newData.VectorSplitValue.as_v3f.y);
                data.VectorSplitZ = MakeVariant(float, newData.VectorSplitValue.as_v3f.z);
                data.VectorSplitW = Variant_Empty;
                break;
            case TypeOf_v4f:
                data.VectorSplitX = MakeVariant(float, newData.VectorSplitValue.as_v4f.x);
                data.VectorSplitY = MakeVariant(float, newData.VectorSplitValue.as_v4f.y);
                data.VectorSplitZ = MakeVariant(float, newData.VectorSplitValue.as_v4f.z);
                data.VectorSplitW = MakeVariant(float, newData.VectorSplitValue.as_v4f.w);
                break;
        }

        if(memcmp(&data.VectorSplitX, &oldData.VectorSplitX, sizeof(float) * 4) != 0) {
            SetVectorSplit(split, data);
        }
    }


}

BeginUnit(MathOperation)
    BeginEnum(MathOperation, false)
        RegisterFlag(MathOperation_Add)
        RegisterFlag(MathOperation_Sub)
        RegisterFlag(MathOperation_Mul)
        RegisterFlag(MathOperation_Div)
        RegisterFlag(MathOperation_Mod)
        RegisterFlag(MathOperation_Log)
        RegisterFlag(MathOperation_Pow)
        RegisterFlag(MathOperation_Cos)
        RegisterFlag(MathOperation_Sin)
        RegisterFlag(MathOperation_Tan)
        RegisterFlag(MathOperation_Atan)
        RegisterFlag(MathOperation_Max)
        RegisterFlag(MathOperation_Min)
    EndEnum()

    BeginComponent(Math)
        RegisterPropertyEnum(u8, MathOperation, MathOperation)
        RegisterProperty(Variant, MathA)
        RegisterProperty(Variant, MathB)
        RegisterProperty(Variant, MathResult)
    EndComponent()

    BeginComponent(VectorSplit)
        RegisterProperty(Variant, VectorSplitX)
        RegisterProperty(Variant, VectorSplitY)
        RegisterProperty(Variant, VectorSplitZ)
        RegisterProperty(Variant, VectorSplitW)
        RegisterProperty(Variant, VectorSplitValue)
    EndComponent()

    RegisterSystem(OnMathChanged, ComponentOf_Math())
    RegisterSystem(OnVectorSplitChanged, ComponentOf_VectorSplit())

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