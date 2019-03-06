#include <Core/Entity.h>
#include <Core/Algorithms.h>
#include <Core/Vector.h>
#include "MemoryStream.h"
#include "Stream.h"
#include "NativeUtils.h"

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>

struct MemoryBuffer {
    eastl::vector<char> Data;
    s32 RefCount;
};

static eastl::unordered_map<const void*, MemoryBuffer> MemoryBuffers;

struct MemoryStream {
    s32 Offset;
    MemoryBuffer* openBuffer;
    bool IsOpen;
};

static bool Delete(Entity entity) {
    StreamClose(entity);
    auto streamData = GetMemoryStreamData(entity);
    if(streamData->openBuffer) {
        streamData->openBuffer->Data.clear();
    }
    streamData->Offset = 0;
    return true;
}

static u64 Read(Entity entity, u64 size, void *data) {
    auto streamData = GetMemoryStreamData(entity);
    if(!streamData->openBuffer) return 0;

    size = Min((u64)streamData->openBuffer->Data.size() - streamData->Offset, size);
    memcpy(data, streamData->openBuffer->Data.data() + streamData->Offset, size);
    streamData->Offset += size;
    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    auto streamData = GetMemoryStreamData(entity);
    if(!streamData->openBuffer) return 0;

    if(streamData->openBuffer->Data.size() < streamData->Offset + size) {
        streamData->openBuffer->Data.resize(streamData->Offset + size);
    }

    memcpy(streamData->openBuffer->Data.data() + streamData->Offset, data, size);

    streamData->Offset += size;

    return size;
}

static s32 Tell(Entity entity) {
    return GetMemoryStreamData(entity)->Offset;
}

static bool Seek(Entity entity, s32 offset) {
    auto vec = GetMemoryStreamData(entity)->openBuffer;
    if(!vec) return false;

    if(offset == StreamSeek_End) {
        offset = vec->Data.size();
    }

    GetMemoryStreamData(entity)->Offset = offset;
    return true;
}

static bool Open(Entity entity, int mode) {
    auto data = GetMemoryStreamData(entity);
    data->Offset = 0;

    if(!data->openBuffer) return false;
    data->IsOpen = true;

    return true;
}

static bool Close(Entity entity) {
    auto data = GetMemoryStreamData(entity);
    data->IsOpen = false;
    data->Offset = 0;
    return true;
}

static bool IsOpen(Entity entity) {
    return GetMemoryStreamData(entity)->IsOpen;
}

API_EXPORT void SetMemoryStreamCapacity(Entity memoryStream, u32 capacity) {
    auto streamData = GetMemoryStreamData(memoryStream);
    if(!streamData->openBuffer) return;

    streamData->openBuffer->Data.resize(capacity);
    streamData->Offset = Min(streamData->Offset, capacity);
}

API_EXPORT NativePtr GetMemoryStreamBuffer(Entity memoryStream) {
    auto streamData = GetMemoryStreamData(memoryStream);
    if(!streamData || !streamData->openBuffer) return NULL;

    return streamData->openBuffer->Data.data();
}

LocalFunction(OnStreamPathChanged, void, Entity entity, StringRef oldPath, StringRef newPath) {
    auto data = GetMemoryStreamData(entity);

    if(!data) return;

    auto it = MemoryBuffers.find(oldPath);
    if(it != MemoryBuffers.end()) {
        it->second.RefCount--;

        if(it->second.RefCount <= 0) {
            MemoryBuffers.erase(it);
        }
    }

    data->openBuffer = NULL;

    if(newPath && strstr(newPath, "memory://")) {
        it = MemoryBuffers.find(newPath);
        if(it == MemoryBuffers.end()) {
            MemoryBuffers[newPath] = MemoryBuffer();
            it = MemoryBuffers.find(newPath);
        }

        it->second.RefCount++;

        data->openBuffer = &it->second;
    }
}

BeginUnit(MemoryStream)
    BeginComponent(MemoryStream)
        RegisterBase(Stream)
    EndComponent()

    RegisterStreamProtocol(MemoryStream, "memory")

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_StreamPath()), OnStreamPathChanged, 0)
EndUnit()
