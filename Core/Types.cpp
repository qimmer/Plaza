//
// Created by Kim on 23/06/2018.
//

#include <Core/NativeUtils.h>
#include "Debug.h"

static StringRef typeNames[] = {
    Intern("Unknown"),

    Intern("void"),
    Intern("u8"),
    Intern("u16"),
    Intern("u32"),
    Intern("u64"),
    Intern("s8"),
    Intern("s16"),
    Intern("s32"),
    Intern("s64"),
    Intern("float"),
    Intern("double"),
    Intern("bool"),
    Intern("StringRef"),

    Intern("v2i"),
    Intern("v3i"),
    Intern("v4i"),

    Intern("v2f"),
    Intern("v3f"),
    Intern("v4f"),

    Intern("Entity"),
    Intern("Type"),

    Intern("rgba8"),
    Intern("rgb8"),

    Intern("rgba32"),
    Intern("rgb32"),

    Intern("Date"),

    Intern("Variant"),

    Intern("NativePtr"),
    Intern("ChildArray")
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
        sizeof(NativePtr),
        sizeof(ChildArray)
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
            alignof(NativePtr),
            alignof(ChildArray)
    };

    Assert(0, type < TypeOf_MAX);

    return typeAlignments[type];
}

API_EXPORT StringRef GetTypeName(Type type) {
    Assert(0, type < TypeOf_MAX);


    return typeNames[type];
}

API_EXPORT Type FindType(StringRef typeName) {
    typeName = Intern(typeName);
    for(auto i = 0; i < TypeOf_MAX; ++i) {
        if(typeName == typeNames[i]) {
            return i;
        }
    }

    return TypeOf_unknown;
}
