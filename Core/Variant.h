//
// Created by Kim Johannsen on 04/01/2018.
//

#ifndef PLAZA_VARIANT_H
#define PLAZA_VARIANT_H

#include <Core/Types.h>
#include <memory.h>

struct ALIGN(16) Variant {
    union {
        m4x4f data;

        m4x4f as_m4x4f;
        m3x3f as_m3x3f;
        v2f as_v2f;
        v3f as_v3f;
        v4f as_v4f;
        v2i as_v2i;
        v3i as_v3i;
        v4i as_v4i;
        float as_float;
        double as_double;
        bool as_bool;
        StringRef as_StringRef;
        s64 as_s64;
        s32 as_s32;
        s16 as_s16;
        s8 as_s8;
        u64 as_u64;
        u32 as_u32;
        u16 as_u16;
        u8 as_u8;
        Type as_Type;
        Entity as_Entity;
        rgba8 as_rgba8;
        rgb8 as_rgb8;
        rgba32 as_rgba32;
        rgb32 as_rgb32;
        Date as_Date;
        void* as_unknown;
    };

    Type type;
};

#define MakeVariant(TYPE, VALUE) __MakeVariant(&(VALUE), TypeOf_ ## TYPE)

inline Variant __MakeVariant(const void *data, Type type) {
    if(type == TypeOf_Variant) {
        return *(const Variant*)data;
    }
    
    Variant v;
    if(data) {
        memcpy(&v.data, data, GetTypeSize(type));
    } else {
        memset(&v.data, 0, sizeof(m4x4f));
    }
    v.type = type;
    return v;
}

static const Variant Variant_Empty = __MakeVariant(0, TypeOf_unknown);

Variant Cast(Variant v, Type type);

#endif //PLAZA_VARIANT_H
