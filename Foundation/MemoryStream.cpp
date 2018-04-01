#include <Core/Entity.h>
#include <Core/Service.h>
#include "MemoryStream.h"

struct MemoryStream {
    std::vector<char> bytes;
    s32 offset;
    bool open;
};

#define ProtocolIdentifier "memory"

DefineComponent(MemoryStream)
    Dependency(Stream)
EndComponent()

static u64 Read(Entity entity, u64 size, void *data) {
    auto streamData = GetMemoryStream(entity);
    size = std::min((u64)streamData->bytes.size() - streamData->offset, size);
    memcpy(data, streamData->bytes.data() + streamData->offset, size);
    streamData->offset += size;
    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    auto streamData = GetMemoryStream(entity);
    if(streamData->bytes.size() < streamData->offset + size) {
        streamData->bytes.resize(streamData->offset + size);
    }

    memcpy(streamData->bytes.data() + streamData->offset, data, size);

    streamData->offset += size;

    return size;
}

static s32 Tell(Entity entity) {
    return GetMemoryStream(entity)->offset;
}

static bool Seek(Entity entity, s32 offset) {
    if(offset == StreamSeek_End) {
        offset = GetMemoryStream(entity)->bytes.size();
    }

    GetMemoryStream(entity)->offset = offset;
    return true;
}

static bool Open(Entity entity, int mode) {
    auto data = GetMemoryStream(entity);
    data->offset = 0;
    return data->open = true;
}

static void Close(Entity entity) {
    auto data = GetMemoryStream(entity);
    data->open = false;
    data->bytes.resize(std::max(data->offset, 0));
    data->offset = 0;
}

static bool IsOpen(Entity entity) {
    return GetMemoryStream(entity)->open;
}

static void OnServiceStart(Service service) {
    StreamProtocol p {
        AddMemoryStream,
        RemoveMemoryStream,
        Seek,
        Tell,
        Read,
        Write,
        IsOpen,
        Open,
        Close
    };

    AddStreamProtocol(ProtocolIdentifier, &p);
}

static void OnServiceStop(Service service){
    RemoveStreamProtocol(ProtocolIdentifier);
}

DefineService(MemoryStream)
    Subscribe(MemoryStreamStarted, OnServiceStart)
    Subscribe(MemoryStreamStopped, OnServiceStop)
EndService()
