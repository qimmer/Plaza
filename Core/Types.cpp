//
// Created by Kim on 23/06/2018.
//

#include <Core/NativeUtils.h>
#include "Debug.h"

static StringRef typeNames[] = {
        "Unknown",

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

        "Date",

        "Variant",

        "NativePtr"
};

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

        sizeof(Date),

        sizeof(Variant),
        sizeof(NativePtr)
    };

    Assert(0, type < TypeOf_MAX);

    return typeSizes[type];
}

API_EXPORT u32 GetTypeAlignment(Type type) {
    static u32 typeAlignments[] = {
            0,

            0,
            alignof(u8),
            alignof(u16),
            alignof(u32),
            alignof(u64),
            alignof(s8),
            alignof(s16),
            alignof(s32),
            alignof(s64),
            alignof(float),
            alignof(double),
            alignof(bool),
            alignof(StringRef),

            alignof(v2i),
            alignof(v3i),
            alignof(v4i),

            alignof(v2f),
            alignof(v3f),
            alignof(v4f),
			
            alignof(Entity),
            alignof(Type),

            alignof(rgba8),
            alignof(rgb8),

            alignof(rgba32),
            alignof(rgb32),

            alignof(Date),

            alignof(Variant),
            alignof(NativePtr)
    };

    Assert(0, type < TypeOf_MAX);

    return typeAlignments[type];
}

API_EXPORT StringRef GetTypeName(Type type) {
    Assert(0, type < TypeOf_MAX);

    return typeNames[type];
}

API_EXPORT Type FindType(StringRef typeName) {
    for(auto i = 0; i < TypeOf_MAX; ++i) {
        if(strcmp(typeName, typeNames[i]) == 0) {
            return i;
        }
    }

    return TypeOf_unknown;
}
