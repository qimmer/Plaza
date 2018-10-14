#include <Core/Entity.h>
#include <Core/Algorithms.h>
#include <Core/Vector.h>
#include "MemoryStream.h"
#include "Stream.h"
#include "NativeUtils.h"

struct MemoryStream {
    Vector(Bytes, char, 256 - 5);
    s32 Offset;
    bool Open;
};

static bool Delete(Entity entity) {
    auto streamData = GetMemoryStreamData(entity);
    VectorClear(streamData->Bytes);
    streamData->Offset = 0;
    return true;
}

static u64 Read(Entity entity, u64 size, void *data) {
    auto streamData = GetMemoryStreamData(entity);
    size = Min((u64)streamData->Bytes.Count - streamData->Offset, size);
    memcpy(data, GetVector(streamData->Bytes) + streamData->Offset, size);
    streamData->Offset += size;
    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    auto streamData = GetMemoryStreamData(entity);
    if(streamData->Bytes.Count < streamData->Offset + size) {
        SetVectorAmount(streamData->Bytes, streamData->Offset + size);
    }

    memcpy(GetVector(streamData->Bytes) + streamData->Offset, data, size);

    streamData->Offset += size;

    return size;
}

static s32 Tell(Entity entity) {
    return GetMemoryStreamData(entity)->Offset;
}

static bool Seek(Entity entity, s32 offset) {
    if(offset == StreamSeek_End) {
        offset = GetMemoryStreamData(entity)->Bytes.Count;
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
    SetVectorAmount(data->Bytes, Max(data->Offset, 0));
    data->Offset = 0;
    return true;
}

static bool IsOpen(Entity entity) {
    return GetMemoryStreamData(entity)->Open;
}

LocalFunction(OnMemoryStreamRemoved, void, Entity component, Entity entity) {
    auto data = GetMemoryStreamData(entity);
    SetVectorAmount(data->Bytes, 0);
}

BeginUnit(MemoryStream)
    BeginComponent(MemoryStream)
        RegisterBase(Stream)
    EndComponent()

    RegisterStreamProtocol(MemoryStream, "memory")

    RegisterSubscription(EventOf_EntityComponentRemoved(), OnMemoryStreamRemoved, ComponentOf_MemoryStream())
EndUnit()
