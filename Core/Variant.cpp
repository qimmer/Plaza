//
// Created by Kim on 24/05/2018.
//

#include <Core/Variant.h>
#include <Core/Types.h>
#include <Core/Entity.h>
#include <Core/Debug.h>
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
        sprintf(buffer, T_FROM ## _sprintf, *(T_FROM*)input);\
        *(StringRef*)output = Intern(buffer);\
    }

#define CastStringToNumber(T_TO) \
    static inline void Cast_StringRef_ ## T_TO (const void *input, void *output) {\
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
        for(auto i = 0; i < Min(V_FROM_CNT, V_TO_CNT); ++i) {\
            Cast_ ## TE_FROM ## _ ## TE_TO (&src_elems[i], &dst_elems[i]);\
        }\
    }

#define CastVectorString(VT) \
    static inline void Cast_ ## VT ## _StringRef(const void *input, void *output) {\
        \
    }\
    static inline void Cast_StringRef_ ## VT (const void *input, void *output) {\
        \
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
    CastVector(v2i, T_FROM, s32, T_FROM, 2, 1)\
    CastVector(v3i, T_FROM, s32, T_FROM, 3, 1)\
    CastVector(v4i, T_FROM, s32, T_FROM, 4, 1)\
    CastVector(v2f, T_FROM, float, T_FROM, 2, 1)\
    CastVector(v3f, T_FROM, float, T_FROM, 3, 1)\
    CastVector(v4f, T_FROM, float, T_FROM, 4, 1)

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

CastVector(v2f, v2i, float, s32, 2, 2)
CastVector(v2f, v3i, float, s32, 2, 3)
CastVector(v2f, v4i, float, s32, 2, 4)
CastVector(v3f, v2i, float, s32, 3, 2)
CastVector(v3f, v3i, float, s32, 3, 3)
CastVector(v3f, v4i, float, s32, 3, 4)
CastVector(v4f, v2i, float, s32, 4, 2)
CastVector(v4f, v3i, float, s32, 4, 3)
CastVector(v4f, v4i, float, s32, 4, 4)

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
CastVector(v3i, v2i, s32, s32, 3, 2)
CastVector(v3i, v4i, s32, s32, 3, 4)
CastVector(v4i, v2i, s32, s32, 4, 2)
CastVector(v4i, v3i, s32, s32, 4, 3)

CastVector(v2f, v3f, float, float, 2, 3)
CastVector(v2f, v4f, float, float, 2, 4)
CastVector(v3f, v2f, float, float, 3, 2)
CastVector(v3f, v4f, float, float, 3, 4)
CastVector(v4f, v2f, float, float, 4, 2)
CastVector(v4f, v3f, float, float, 4, 3)

CastVectorString(v2i)
CastVectorString(v3i)
CastVectorString(v4i)

CastVectorString(v2f)
CastVectorString(v3f)
CastVectorString(v4f)

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
                default:\
                    Log(0, LogSeverity_Error, "Cannot cast variant from %s to %s", GetTypeName(type_from), GetTypeName(type_to));\
                    return Variant_Empty;\
            }\
            break;\
        }


API_EXPORT Variant Cast(Variant v, Type type_to) {
    auto type_from = v.type;
    if(type_from == type_to) return v;

    Variant newVar;
    memset(&newVar.data, 0, sizeof(m4x4f));
    newVar.type = type_to;

    auto input = (const void*)&v.data;
    auto output = (void*)&newVar.data;

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
        default:
            Log(0, LogSeverity_Error, "Cannot cast variant from %s to %s", GetTypeName(type_from), GetTypeName(type_to));
            return Variant_Empty;
    }

    return newVar;
}
