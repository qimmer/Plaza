//
// Created by Kim on 24/05/2018.
//

#include <Core/Variant.h>
#include <Core/Types.h>
#include <Core/Entity.h>
#include <Core/Debug.h>
#include <Core/Identification.h>
#include <cstring>
#include "Variant.h"
#include <Core/Algorithms.h>


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
#define bool_sprintf "%d"

#define CastNumberToString(T_FROM) \
    static inline void Cast_ ## T_FROM ## _StringRef (const void *input, void *output) {\
        char buffer[128];\
        if(TypeOf_ ## T_FROM == TypeOf_bool) {\
            *(StringRef*)output = (*(const bool*)input) ? "true" : "false";\
            return;\
        }\
        sprintf(buffer, T_FROM ## _sprintf, *(T_FROM*)input);\
        *(StringRef*)output = Intern(buffer);\
    }

#define CastStringToNumber(T_TO) \
    static inline void Cast_StringRef_ ## T_TO (const void *input, void *output) {\
        if(TypeOf_ ## T_TO == TypeOf_bool) {\
            *(bool*)output = (*(StringRef*)input)[0]=='t';\
            return;\
        }\
        sscanf(*(StringRef*)input, T_TO ## _sprintf, output);\
    }

#define CastNumberToNumber(T_FROM, T_TO)\
    static inline void Cast_ ## T_FROM ## _ ## T_TO (const void *input, void *output) {\
        *(T_TO*)output = (T_TO)*(T_FROM*)input;\
    }

#define CastSame(T_FROM)\
    static inline void Cast_ ## T_FROM ## _ ## T_FROM (const void *input, void *output) {\
    }

static inline void Cast_StringRef_StringRef(const void *input, void *output) {}

#define CastVector(T_FROM, T_TO, TE_FROM, TE_TO, V_FROM_CNT, V_TO_CNT) \
    static inline void Cast_ ## TE_FROM ## _ ## TE_TO(const void *input, void *output);\
    static inline void Cast_ ## T_FROM ## _ ## T_TO (const void *input, void *output) {\
        auto src_elems = (TE_FROM*)input;\
        auto dst_elems = (TE_TO*)output;\
        for(auto i = 0; i < V_TO_CNT; ++i) {\
            if(i < V_FROM_CNT) {\
                Cast_ ## TE_FROM ## _ ## TE_TO (&src_elems[i], &dst_elems[i]);\
            } else {\
                if(V_FROM_CNT == 1) {\
                    dst_elems[i] = src_elems[0];\
                } else if (i == 3) {\
                    dst_elems[i] = 1;\
                } else {\
                    dst_elems[i] = 0;\
                }\
            }\
        }\
    }

#define CastVectorString(VT, SCALART, FORMAT) \
    static inline void Cast_ ## VT ## _StringRef(const void *input, void *output) {\
        auto scalars = (const SCALART*)input;\
        char buffer[256];\
        sprintf(buffer, FORMAT, scalars[0], scalars[1], scalars[2], scalars[3]);\
        *(StringRef*)output = Intern(buffer);\
    }\
    static inline void Cast_StringRef_ ## VT (const void *input, void *output) {\
        auto scalars = (SCALART*)output;\
        sscanf(*(StringRef*)input, FORMAT, &scalars[0], &scalars[1], &scalars[2], &scalars[3]);\
    }
#define CastNumber(T_FROM) \
    CastNumberToNumber(T_FROM, u8)\
    CastNumberToNumber(T_FROM, u16)\
    CastNumberToNumber(T_FROM, u32)\
    CastNumberToNumber(T_FROM, u64)\
    CastNumberToNumber(T_FROM, s8)\
    CastNumberToNumber(T_FROM, s16)\
    CastNumberToNumber(T_FROM, s32)\
    CastNumberToNumber(T_FROM, s64)\
    CastNumberToNumber(T_FROM, float)\
    CastNumberToNumber(T_FROM, double)\
    CastNumberToNumber(T_FROM, bool)\
    CastNumberToString(T_FROM)\
    CastStringToNumber(T_FROM)\
    CastVector(T_FROM, v2i, T_FROM, s32, 1, 2)\
    CastVector(T_FROM, v3i, T_FROM, s32, 1, 3)\
    CastVector(T_FROM, v4i, T_FROM, s32, 1, 4)\
    CastVector(T_FROM, v2f, T_FROM, float, 1, 2)\
    CastVector(T_FROM, v3f, T_FROM, float, 1, 3)\
    CastVector(T_FROM, v4f, T_FROM, float, 1, 4)\
    CastVector(T_FROM, rgba8, T_FROM, u8, 1, 4)\
    CastVector(T_FROM, rgba32, T_FROM, float, 1, 4)\
    CastVector(v2i, T_FROM, s32, T_FROM, 2, 1)\
    CastVector(v3i, T_FROM, s32, T_FROM, 3, 1)\
    CastVector(v4i, T_FROM, s32, T_FROM, 4, 1)\
    CastVector(v2f, T_FROM, float, T_FROM, 2, 1)\
    CastVector(v3f, T_FROM, float, T_FROM, 3, 1)\
    CastVector(v4f, T_FROM, float, T_FROM, 4, 1)\
    CastVector(rgba8, T_FROM, float, T_FROM, 4, 1)\
    CastVector(rgba32, T_FROM, float, T_FROM, 4, 1)

CastNumber(u8)
CastNumber(u16)
CastNumber(u32)
CastNumber(u64)
CastNumber(s8)
CastNumber(s16)
CastNumber(s32)
CastNumber(s64)
CastNumber(float)
CastNumber(double)
CastNumber(bool)

CastSame(v2i)
CastSame(v3i)
CastSame(v4i)
CastSame(v2f)
CastSame(v3f)
CastSame(v4f)
CastSame(rgba8)
CastSame(rgba32)

CastVector(v2f, v2i, float, s32, 2, 2)
CastVector(v2f, v3i, float, s32, 2, 3)
CastVector(v2f, v4i, float, s32, 2, 4)
CastVector(v2f, rgba8, float, u8, 2, 4)
CastVector(v2f, rgba32, float, float, 2, 4)
CastVector(v3f, v2i, float, s32, 3, 2)
CastVector(v3f, v3i, float, s32, 3, 3)
CastVector(v3f, v4i, float, s32, 3, 4)
CastVector(v3f, rgba8, float, u8, 3, 4)
CastVector(v3f, rgba32, float, float, 3, 4)
CastVector(v4f, v2i, float, s32, 4, 2)
CastVector(v4f, v3i, float, s32, 4, 3)
CastVector(v4f, v4i, float, s32, 4, 4)
CastVector(v4f, rgba8, float, u8, 4, 4)
CastVector(v4f, rgba32, float, float, 4, 4)

CastVector(v2i, v2f, s32, float, 2, 2)
CastVector(v2i, v3f, s32, float, 2, 3)
CastVector(v2i, v4f, s32, float, 2, 4)
CastVector(v3i, v2f, s32, float, 3, 2)
CastVector(v3i, v3f, s32, float, 3, 3)
CastVector(v3i, v4f, s32, float, 3, 4)
CastVector(v4i, v2f, s32, float, 4, 2)
CastVector(v4i, v3f, s32, float, 4, 3)
CastVector(v4i, v4f, s32, float, 4, 4)

CastVector(v2i, v3i, s32, s32, 2, 3)
CastVector(v2i, v4i, s32, s32, 2, 4)
CastVector(v2i, rgba8, s32, u8, 2, 4)
CastVector(v2i, rgba32, s32, float, 2, 4)
CastVector(v3i, v2i, s32, s32, 3, 2)
CastVector(v3i, v4i, s32, s32, 3, 4)
CastVector(v3i, rgba8, s32, u8, 3, 4)
CastVector(v3i, rgba32, s32, float, 3, 4)
CastVector(v4i, v2i, s32, s32, 4, 2)
CastVector(v4i, v3i, s32, s32, 4, 3)
CastVector(v4i, rgba8, s32, u8, 4, 4)
CastVector(v4i, rgba32, s32, float, 4, 4)
CastVector(rgba8, v2i, u8, s32, 4, 2)
CastVector(rgba8, v3i, u8, s32, 4, 3)
CastVector(rgba8, v4i, u8, s32, 4, 4)
CastVector(rgba32, v2i, float, s32, 4, 2)
CastVector(rgba32, v3i, float, s32, 4, 3)
CastVector(rgba32, v4i, float, s32, 4, 4)

CastVector(v2f, v3f, float, float, 2, 3)
CastVector(v2f, v4f, float, float, 2, 4)
CastVector(v3f, v2f, float, float, 3, 2)
CastVector(v3f, v4f, float, float, 3, 4)
CastVector(v4f, v2f, float, float, 4, 2)
CastVector(v4f, v3f, float, float, 4, 3)
CastVector(rgba8, v2f, u8, float, 4, 2)
CastVector(rgba8, v3f, u8, float, 4, 3)
CastVector(rgba8, v4f, u8, float, 4, 4)
CastVector(rgba8, rgba32, u8, float, 4, 4)
CastVector(rgba32, v2f, float, float, 4, 2)
CastVector(rgba32, v3f, float, float, 4, 3)
CastVector(rgba32, v4f, float, float, 4, 4)
CastVector(rgba32, rgba8, float, u8, 4, 4)

CastVectorString(v2i, s32, "[%d,%d]")
CastVectorString(v3i, s32, "[%d,%d,%d]")
CastVectorString(v4i, s32, "[%d,%d,%d,%d]")

CastVectorString(v2f, float, "[%f,%f]")
CastVectorString(v3f, float, "[%f,%f,%f]")
CastVectorString(v4f, float, "[%f,%f,%f,%f]")
CastVectorString(rgba32, float, "[%f,%f,%f,%f]")

CastVectorString(rgba8, u8, "#%02hhX%02hhX%02hhX%02hhX")

#define Cast_Case(FROM_T, TO_T) case TO_T: Cast_ ## FROM_T ## _ ## TO_T (input, output); break;

#define Cast_Inner(FROM_T, TO_T)\
    case TypeOf_ ## FROM_T: Cast_ ## FROM_T ## _ ## TO_T (input, output); break;

#define Cast_Outer(TO_T)\
    case TypeOf_ ## TO_T:\
        {\
            switch(type_from) {\
                Cast_Inner(u8, TO_T)\
                Cast_Inner(u16, TO_T)\
                Cast_Inner(u32, TO_T)\
                Cast_Inner(u64, TO_T)\
                Cast_Inner(s8, TO_T)\
                Cast_Inner(s16, TO_T)\
                Cast_Inner(s32, TO_T)\
                Cast_Inner(s64, TO_T)\
                Cast_Inner(double, TO_T)\
                Cast_Inner(float, TO_T)\
                Cast_Inner(bool, TO_T)\
                Cast_Inner(StringRef, TO_T)\
                Cast_Inner(v2i, TO_T)\
                Cast_Inner(v3i, TO_T)\
                Cast_Inner(v4i, TO_T)\
                Cast_Inner(v2f, TO_T)\
                Cast_Inner(v3f, TO_T)\
                Cast_Inner(v4f, TO_T)\
                Cast_Inner(rgba8, TO_T)\
                Cast_Inner(rgba32, TO_T)\
                default:\
                    Log(0, LogSeverity_Error, "Cannot cast variant from %s to %s", GetTypeName(type_from), GetTypeName(type_to));\
                    return Variant_Empty;\
            }\
            break;\
        }


API_EXPORT Variant Cast(Variant v, Type type_to) {
    auto type_from = v.type;
    if(type_from == type_to || type_to == TypeOf_Variant) return v;
    if(type_to == TypeOf_unknown) {
        return Variant_Empty;
    }

    Variant newVar;
    memset(&newVar.data, 0, sizeof(m4x4f));
    newVar.type = type_to;

    if(!v.type) return newVar; // Return default value if source is unknown

    auto input = (const void*)&v.data;
    auto output = (void*)&newVar.data;

    if(type_to == TypeOf_Entity && type_from == TypeOf_StringRef) {
        auto result = FindEntityByUuid(v.as_StringRef);
        return MakeVariant(Entity, result);
    }

    if(type_to == TypeOf_StringRef && type_from == TypeOf_Entity) {
        auto result = GetUuid(v.as_Entity);
        return MakeVariant(StringRef, result);
    }

    switch(type_to) {
        Cast_Outer(u8)
        Cast_Outer(u16)
        Cast_Outer(u32)
        Cast_Outer(u64)
        Cast_Outer(s8)
        Cast_Outer(s16)
        Cast_Outer(s32)
        Cast_Outer(s64)
        Cast_Outer(float)
        Cast_Outer(double)
        Cast_Outer(bool)
        Cast_Outer(StringRef)
        Cast_Outer(v2i)
        Cast_Outer(v3i)
        Cast_Outer(v4i)
        Cast_Outer(v2f)
        Cast_Outer(v3f)
        Cast_Outer(v4f)
        Cast_Outer(rgba8)
        Cast_Outer(rgba32)
        default:
            Log(0, LogSeverity_Error, "Cannot cast variant from %s to %s", GetTypeName(type_from), GetTypeName(type_to));
            return Variant_Empty;
    }

    return newVar;
}
