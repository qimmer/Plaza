//
// Created by Kim on 23/06/2018.
//

#include <Core/Types.h>
#include "Debug.h"

API_EXPORT u32 GetTypeSize(Type type) {
    static u32 typeSizes[] = {
        0,

        0,
        sizeof(u8),
        sizeof(u16),
        sizeof(u32),
        sizeof(u64),
        sizeof(s8),
        sizeof(s16),
        sizeof(s32),
        sizeof(s64),
        sizeof(float),
        sizeof(double),
        sizeof(bool),
        sizeof(StringRef),

        sizeof(v2i),
        sizeof(v3i),
        sizeof(v4i),

        sizeof(v2f),
        sizeof(v3f),
        sizeof(v4f),

        sizeof(Entity),
        sizeof(Type),

        sizeof(rgba8),
        sizeof(rgb8),

        sizeof(rgba32),
        sizeof(rgb32),

        sizeof(Variant)
    };

    Assert(0, type < TypeOf_MAX);

    return typeSizes[type];
}

API_EXPORT StringRef GetTypeName(Type type) {
    static StringRef typeNames[] = {
        "",

        "void",
        "u8",
        "u16",
        "u32",
        "u64",
        "s8",
        "s16",
        "s32",
        "s64",
        "float",
        "double",
        "bool",
        "StringRef",

        "v2i",
        "v3i",
        "v4i",

        "v2f",
        "v3f",
        "v4f",

        "Entity",
        "Type",

        "rgba8",
        "rgb8",

        "rgba32",
        "rgb32",

        "Variant"
    };

    Assert(0, type < TypeOf_MAX);

    return typeNames[type];
}
