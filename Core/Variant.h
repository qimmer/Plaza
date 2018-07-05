//
// Created by Kim Johannsen on 04/01/2018.
//

#ifndef PLAZA_VARIANT_H
#define PLAZA_VARIANT_H

#include <Core/NativeUtils.h>

struct ALIGN(16) Variant {
    Entity type;
    char buffer[1024 - sizeof(Entity)];
};

bool GetVariant(Variant v, u32 bufferSize, void *buffer, u32 *writtenBytes);

#define SetVariant(V, TYPE, VALUE) if(TypeOf_ ## TYPE == TypeOf_String) { V.string = VALUE; } else { auto val = VALUE; memcpy(&v.buffer, &val, sizeof(val)); }
Variant CreateVariant(Entity type, u32 bufferSize, const void *buffer);

#endif //PLAZA_VARIANT_H
