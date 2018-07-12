//
// Created by Kim Johannsen on 04/01/2018.
//

#ifndef PLAZA_VARIANT_H
#define PLAZA_VARIANT_H

#include <Core/NativeUtils.h>

struct ALIGN(16) Variant {
    char buffer[256 - sizeof(Type)];
    Type type;
};

bool GetVariant(Variant v, u32 bufferSize, void *buffer, u32 *writtenBytes);

#define SetVariant(V, TYPE, VALUE) v.type = TypeOf_ ## TYPE; if(TypeOf_ ## TYPE == TypeOf_String) { strncpy(v.buffer, sizeof(v.buffer), VALUE); } else { auto val = VALUE; memcpy(v.buffer, &val, sizeof(val)); }
Variant CreateVariant(Type type, u32 bufferSize, const void *buffer);

#endif //PLAZA_VARIANT_H
