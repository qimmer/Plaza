#include <Core/Entity.h>
#include <Core/Math.h>
#include <Core/Vector.h>
#include "MemoryStream.h"
#include "Stream.h"
#include "NativeUtils.h"

struct MemoryStream {
    Vector(Bytes, char, 256 - 5);
    s32 Offset;
    bool Open;
};

static u64 Read(Entity entity, u64 size, void *data) {
    auto streamData = GetMemoryStreamData(entity);
    size = Min((u64)streamData->NumBytes - streamData->Offset, size);
    memcpy(data, streamData->Bytes + streamData->Offset, size);
    streamData->Offset += size;
    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    auto streamData = GetMemoryStreamData(entity);
    if(streamData->NumBytes < streamData->Offset + size) {
        SetVectorAmount(streamData, Bytes, streamData->Offset + size);
    }

    memcpy(streamData->Bytes + streamData->Offset, data, size);

    streamData->Offset += size;

    return size;
}

static s32 Tell(Entity entity) {
    return GetMemoryStreamData(entity)->Offset;
}

static bool Seek(Entity entity, s32 offset) {
    if(offset == StreamSeek_End) {
        offset = GetMemoryStreamData(entity)->NumBytes;
    }

    GetMemoryStreamData(entity)->Offset = offset;
    return true;
}

static bool Open(Entity entity, int mode) {
    auto data = GetMemoryStreamData(entity);
    data->Offset = 0;
    return data->Open = true;
}

static bool Close(Entity entity) {
    auto data = GetMemoryStreamData(entity);
    data->Open = false;
    SetVectorAmount(data, Bytes, Max(data->Offset, 0));
    data->Offset = 0;
    return true;
}

static bool IsOpen(Entity entity) {
    return GetMemoryStreamData(entity)->Open;
}


BeginUnit(MemoryStream)
    BeginComponent(MemoryStream)
        RegisterBase(Stream)
    EndComponent()

    RegisterStreamProtocol(MemoryStream, "memory")
EndUnit()
