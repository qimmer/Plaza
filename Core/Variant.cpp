//
// Created by Kim on 24/05/2018.
//

#include <Core/Variant.h>
#include <Core/Types.h>
#include <Core/Entity.h>
#include <Core/Debug.h>
#include <cstring>

BeginUnit(Variant)
EndUnit()

API_EXPORT bool GetVariant(Variant v, u32 bufferSize, void *buffer, u32 *writtenBytesOut) {
    if(!IsEntityValid(v.type)) return false;

    auto typeSize = GetTypeSize(v.type);

    if(v.type == TypeOf_StringRef) {
        Assert(0, bufferSize > strlen(v.buffer));
        strcpy((char*)buffer, v.buffer);
    } else {
        memcpy(buffer, v.buffer, typeSize);
    }

    return true;
}

API_EXPORT Variant CreateVariant(Entity type, u32 bufferSize, const void *buffer) {
    auto typeSize = GetTypeSize(type);
    Assert(0, bufferSize >= typeSize);

    Variant v;
    v.type = type;

    if(type == TypeOf_StringRef) {
        strcpy(v.buffer, *((const char**)buffer));
    } else {
        memcpy(v.buffer, buffer, typeSize);
    }

    return v;
}
