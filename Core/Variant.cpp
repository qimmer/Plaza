//
// Created by Kim on 24/05/2018.
//

#include <Core/Variant.h>
#include <Core/Types.h>
#include <Core/Entity.h>
#include <Core/Debug.h>
#include <cstring>
#include "Variant.h"


BeginUnit(Variant)
EndUnit()

#define s8_sprintf "%hh"
#define s16_sprintf "%h"
#define s32_sprintf "%l"
#define s64_sprintf "%ll"
#define u8_sprintf "%hhu"
#define u16_sprintf "%hu"
#define u32_sprintf "%lu"
#define u64_sprintf "%llu"
#define float_sprintf "%f"
#define double_sprintf "%f"

#define CastStringCase(T_TO) sscanf(v.as_StringRef, T_TO ## _sprintf, &newVar.as_ ## T_TO); return newVar; break;
#define CastNumberCase(T_FROM, T_TO) newVar.as_ ## T_TO = (T_TO)v.as_ ## T_FROM; return newVar; break

#define CastNumber(T_TO) \
    {\
        switch(v.type) {\
            case TypeOf_s8: CastNumberCase(s8, T_TO);\
            case TypeOf_s16: CastNumberCase(s16, T_TO);\
            case TypeOf_s32: CastNumberCase(s32, T_TO);\
            case TypeOf_s64: CastNumberCase(s64, T_TO);\
            case TypeOf_u8: CastNumberCase(u8, T_TO);\
            case TypeOf_u16: CastNumberCase(u16, T_TO);\
            case TypeOf_u32: CastNumberCase(u32, T_TO);\
            case TypeOf_u64: CastNumberCase(u64, T_TO);\
            case TypeOf_float: CastNumberCase(float, T_TO);\
            case TypeOf_double: CastNumberCase(double, T_TO);\
            case TypeOf_bool: CastNumberCase(bool, T_TO);\
            case TypeOf_StringRef: CastStringCase(T_TO);\
            case TypeOf_Variant: newVar.as_ ## T_TO = v.as_ ## T_TO; return newVar; break;\
        }\
        break;\
    }

#define CastBool() \
    {\
        switch(v.type) {\
            case TypeOf_s8: CastNumberCase(s8, bool);\
            case TypeOf_s16: CastNumberCase(s16, bool);\
            case TypeOf_s32: CastNumberCase(s32, bool);\
            case TypeOf_s64: CastNumberCase(s64, bool);\
            case TypeOf_u8: CastNumberCase(u8, bool);\
            case TypeOf_u16: CastNumberCase(u16, bool);\
            case TypeOf_u32: CastNumberCase(u32, bool);\
            case TypeOf_u64: CastNumberCase(u64, bool);\
            case TypeOf_float: CastNumberCase(float, bool);\
            case TypeOf_double: CastNumberCase(double, bool);\
            case TypeOf_StringRef: newVar.as_bool = tolower(v.as_StringRef[0]) == 't'; return newVar; break;\
            case TypeOf_Variant: newVar.as_bool = v.as_u64 != 0; return newVar; break;\
        }\
        break;\
    }


#define CastString() \
    {\
        switch(v.type) {\
            case TypeOf_s8: CastStringCase(s8);\
            case TypeOf_s16: CastStringCase(s16);\
            case TypeOf_s32: CastStringCase(s32);\
            case TypeOf_s64: CastStringCase(s64);\
            case TypeOf_u8: CastStringCase(u8);\
            case TypeOf_u16: CastStringCase(u16);\
            case TypeOf_u32: CastStringCase(u32);\
            case TypeOf_u64: CastStringCase(u64);\
            case TypeOf_float: CastStringCase(float);\
            case TypeOf_double: CastStringCase(double);\
            case TypeOf_bool: {\
                newVar.as_StringRef = v.as_bool ? Intern("true") : Intern("false");\
                return newVar;\
            }\
        }\
        break;\
    }

#define CastVec2(T_TO, ELEM_TO) \
    {\
        switch(v.type) {\
            case TypeOf_s8: newVar.as_ ## T_TO = { (ELEM_TO)v.as_s8, (ELEM_TO)v.as_s8 }; return newVar; break;\
            case TypeOf_s16: newVar.as_ ## T_TO = { (ELEM_TO)v.as_s16, (ELEM_TO)v.as_s16 }; return newVar; break;\
            case TypeOf_s32: newVar.as_ ## T_TO = { (ELEM_TO)v.as_s32, (ELEM_TO)v.as_s32 }; return newVar; break;\
            case TypeOf_s64: newVar.as_ ## T_TO = { (ELEM_TO)v.as_s64, (ELEM_TO)v.as_s64 }; return newVar; break;\
            case TypeOf_u8: newVar.as_ ## T_TO = { (ELEM_TO)v.as_u8, (ELEM_TO)v.as_u8 }; return newVar; break;\
            case TypeOf_u16: newVar.as_ ## T_TO = { (ELEM_TO)v.as_u16, (ELEM_TO)v.as_u16 }; return newVar; break;\
            case TypeOf_u32: newVar.as_ ## T_TO = { (ELEM_TO)v.as_u32, (ELEM_TO)v.as_u32 }; return newVar; break;\
            case TypeOf_u64: newVar.as_ ## T_TO = { (ELEM_TO)v.as_u64, (ELEM_TO)v.as_u64 }; return newVar; break;\
            case TypeOf_float: newVar.as_ ## T_TO = { (ELEM_TO)v.as_float, (ELEM_TO)v.as_float }; return newVar; break;\
            case TypeOf_double: newVar.as_ ## T_TO = { (ELEM_TO)v.as_double, (ELEM_TO)v.as_double }; return newVar; break;\
            case TypeOf_bool: newVar.as_ ## T_TO = { (ELEM_TO)v.as_bool, (ELEM_TO)v.as_bool }; return newVar; break;\
            case TypeOf_v2i: newVar.as_ ## T_TO = { (ELEM_TO)v.as_v2i.x, (ELEM_TO)v.as_v2i.y }; return newVar; break;\
            case TypeOf_v2f: newVar.as_ ## T_TO = { (ELEM_TO)v.as_v2f.x, (ELEM_TO)v.as_v2f.y }; return newVar; break;\
        }\
        break;\
    }
#define CastVec3(T_TO, ELEM_TO) \
    {\
        switch(v.type) {\
            case TypeOf_s8: newVar.as_ ## T_TO = { (ELEM_TO)v.as_s8, (ELEM_TO)v.as_s8, (ELEM_TO)v.as_s8 }; return newVar; break;\
            case TypeOf_s16: newVar.as_ ## T_TO = { (ELEM_TO)v.as_s16, (ELEM_TO)v.as_s16, (ELEM_TO)v.as_s16 }; return newVar; break;\
            case TypeOf_s32: newVar.as_ ## T_TO = { (ELEM_TO)v.as_s32, (ELEM_TO)v.as_s32, (ELEM_TO)v.as_s32 }; return newVar; break;\
            case TypeOf_s64: newVar.as_ ## T_TO = { (ELEM_TO)v.as_s64, (ELEM_TO)v.as_s64, (ELEM_TO)v.as_s64 }; return newVar; break;\
            case TypeOf_u8: newVar.as_ ## T_TO = { (ELEM_TO)v.as_u8, (ELEM_TO)v.as_u8, (ELEM_TO)v.as_u8 }; return newVar; break;\
            case TypeOf_u16: newVar.as_ ## T_TO = { (ELEM_TO)v.as_u16, (ELEM_TO)v.as_u16, (ELEM_TO)v.as_u16 }; return newVar; break;\
            case TypeOf_u32: newVar.as_ ## T_TO = { (ELEM_TO)v.as_u32, (ELEM_TO)v.as_u32, (ELEM_TO)v.as_u32 }; return newVar; break;\
            case TypeOf_u64: newVar.as_ ## T_TO = { (ELEM_TO)v.as_u64, (ELEM_TO)v.as_u64, (ELEM_TO)v.as_u64 }; return newVar; break;\
            case TypeOf_float: newVar.as_ ## T_TO = { (ELEM_TO)v.as_float, (ELEM_TO)v.as_float, (ELEM_TO)v.as_float }; return newVar; break;\
            case TypeOf_double: newVar.as_ ## T_TO = { (ELEM_TO)v.as_double, (ELEM_TO)v.as_double, (ELEM_TO)v.as_double }; return newVar; break;\
            case TypeOf_bool: newVar.as_ ## T_TO = { (ELEM_TO)v.as_bool, (ELEM_TO)v.as_bool, (ELEM_TO)v.as_bool }; return newVar; break;\
            case TypeOf_v3i: newVar.as_ ## T_TO = { (ELEM_TO)v.as_v3i.x, (ELEM_TO)v.as_v3i.y, (ELEM_TO)v.as_v3i.z }; return newVar; break;\
            case TypeOf_v3f: newVar.as_ ## T_TO = { (ELEM_TO)v.as_v3f.x, (ELEM_TO)v.as_v3f.y, (ELEM_TO)v.as_v3f.z }; return newVar; break;\
        }\
        break;\
    }

API_EXPORT Variant Cast(Variant v, Type type) {
    if(type == v.type) return v;

    Variant newVar;
    memset(&newVar.data, 0, sizeof(m4x4f));
    newVar.type = type;

    switch(type) {
        case TypeOf_s8: CastNumber(s8)
        case TypeOf_s16: CastNumber(s16)
        case TypeOf_s32: CastNumber(s32)
        case TypeOf_s64: CastNumber(s64)
        case TypeOf_u8: CastNumber(u8)
        case TypeOf_u16: CastNumber(u16)
        case TypeOf_u32: CastNumber(u32)
        case TypeOf_u64: CastNumber(u64)
        case TypeOf_float: CastNumber(float)
        case TypeOf_double: CastNumber(double)
        case TypeOf_bool: CastBool()
        case TypeOf_StringRef: CastString()
        case TypeOf_v2i: CastVec2(v2i, int)
        case TypeOf_v2f: CastVec2(v2f, float)
        case TypeOf_v3i: CastVec3(v3i, int)
        case TypeOf_v3f: CastVec3(v3f, float)
    }

    Log(0, LogSeverity_Error, "Cannot cast variant from %s to %s", GetTypeName(v.type), GetTypeName(type));

    return newVar;
}
