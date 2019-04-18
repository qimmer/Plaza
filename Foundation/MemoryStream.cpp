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
    eastl::fixed_vector<char, 1024> Data;
};

static bool Delete(Entity entity) {
    StreamClose(entity);
    auto streamData = GetMemoryStream(entity);

    if (streamData.buffer) {
        streamData.buffer = 0;
    }
    streamData.Offset = 0;
    SetMemoryStream(entity, streamData);

    return true;
}

static u64 Read(Entity entity, u64 size, void *data) {
    auto streamData = GetMemoryStream(entity);
    if (!streamData.buffer) return 0;

    size = Min((u64)streamData.buffer->Data.size() - streamData.Offset, size);
    memcpy(data, streamData.buffer->Data.data() + streamData.Offset, size);
    streamData.Offset += size;
    SetMemoryStream(entity, streamData);
    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    auto streamData = GetMemoryStream(entity);
    if (!streamData.buffer) return 0;

    if (streamData.buffer->Data.size() < streamData.Offset + size) {
        streamData.buffer->Data.resize(streamData.Offset + size);
    }

    memcpy(streamData.buffer->Data.data() + streamData.Offset, data, size);

    streamData.Offset += size;
    SetMemoryStream(entity, streamData);

    return size;
}

static s32 Tell(Entity entity) {
	auto data = GetMemoryStream(entity);
    return data.Offset;
}

static bool Seek(Entity entity, s32 offset) {
	auto data = GetMemoryStream(entity);
    auto vec = data.buffer;
    if (!vec) return false;

    if (offset == StreamSeek_End) {
        offset = vec->Data.size();
    }

    data.Offset = offset;
    SetMemoryStream(entity, data);
    return true;
}

static bool Open(Entity entity, int mode) {
    auto data = GetMemoryStream(entity);
    data.Offset = 0;

    if (!data.buffer) {
        data.buffer = new MemoryBuffer();
    }

    data.IsOpen = true;
    SetMemoryStream(entity, data);

    return true;
}

static bool Close(Entity entity) {
    auto data = GetMemoryStream(entity);
    if(GetStreamMode(entity) == StreamMode_Write) {
        data.buffer->Data.resize(data.Offset);
    }

    data.IsOpen = false;
    data.Offset = 0;
    SetMemoryStream(entity, data);
    return true;
}

static bool IsOpen(Entity entity) {
	auto data = GetMemoryStream(entity);
    return data.IsOpen;
}

API_EXPORT void SetMemoryStreamCapacity(Entity memoryStream, u32 capacity) {
    auto streamData = GetMemoryStream(memoryStream);
    streamData.Offset = Min(streamData.Offset, capacity);
    SetMemoryStream(memoryStream, streamData);

    if (!streamData.buffer) return;

    streamData.buffer->Data.resize(capacity);
}

API_EXPORT NativePtr GetMemoryStreamBuffer(Entity memoryStream) {
    auto streamData = GetMemoryStream(memoryStream);
    if(!streamData.buffer) return NULL;

    return streamData.buffer->Data.data();
}

static void OnMemoryStreamChanged(Entity entity, const MemoryStream& oldData, const MemoryStream& newData) {
	if (oldData.buffer && !newData.buffer) {
		delete oldData.buffer;
	}
}

BeginUnit(MemoryStream)
    BeginComponent(MemoryStream)
        RegisterBase(Stream)
    EndComponent()

    RegisterStreamProtocol(Memory, "memory")
    RegisterSystem(OnMemoryStreamChanged, ComponentOf_MemoryStream())
EndUnit()
