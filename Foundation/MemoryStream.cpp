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
    Vector<char, 1024> Data;
};

struct MemoryStream {
    s32 Offset;
    MemoryBuffer* buffer;
    bool IsOpen;
};

static bool Delete(Entity entity) {
    StreamClose(entity);
    auto streamData = GetMemoryStreamData(entity);
	if (streamData) {
		if (streamData->buffer) {
			delete streamData->buffer;
			streamData->buffer = 0;
		}
		streamData->Offset = 0;

		return true;
	}
    
    return false;
}

static u64 Read(Entity entity, u64 size, void *data) {
    auto streamData = GetMemoryStreamData(entity);
	if (streamData) {
		if (!streamData->buffer) return 0;

		size = Min((u64)streamData->buffer->Data.size() - streamData->Offset, size);
		memcpy(data, streamData->buffer->Data.data() + streamData->Offset, size);
		streamData->Offset += size;
		return size;
	}
    
	return 0;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    auto streamData = GetMemoryStreamData(entity);
	if (streamData) {
		if (!streamData->buffer) return 0;

		if (streamData->buffer->Data.size() < streamData->Offset + size) {
			streamData->buffer->Data.resize(streamData->Offset + size);
		}

		memcpy(streamData->buffer->Data.data() + streamData->Offset, data, size);

		streamData->Offset += size;

		return size;
	}
    
	return 0;
}

static s32 Tell(Entity entity) {
	auto data = GetMemoryStreamData(entity);
	if (data) {
		return data->Offset;
	}
    
	return -1;
}

static bool Seek(Entity entity, s32 offset) {
	auto data = GetMemoryStreamData(entity);
	if (data) {
		auto vec = data->buffer;
		if (!vec) return false;

		if (offset == StreamSeek_End) {
			offset = vec->Data.size();
		}

		data->Offset = offset;
		return true;
	}
    
	return false;
}

static bool Open(Entity entity, int mode) {
    auto data = GetMemoryStreamData(entity);
	if (data) {
		data->Offset = 0;

		if (!data->buffer) return false;
		data->IsOpen = true;

		return true;
	}

	return false;
}

static bool Close(Entity entity) {
    auto data = GetMemoryStreamData(entity);
	if (data) {
		if(GetStreamMode(entity) == StreamMode_Write) {
			data->buffer->Data.resize(data->Offset);
		}

		data->IsOpen = false;
		data->Offset = 0;
	}
    return true;
}

static bool IsOpen(Entity entity) {
	auto data = GetMemoryStreamData(entity);
	if (data) {
		return data->IsOpen;
	}

	return false;
}

API_EXPORT void SetMemoryStreamCapacity(Entity memoryStream, u32 capacity) {
    auto streamData = GetMemoryStreamData(memoryStream);
	if (streamData) {
		if (!streamData->buffer) return;

		streamData->buffer->Data.resize(capacity);
		streamData->Offset = Min(streamData->Offset, capacity);
	}
}

API_EXPORT NativePtr GetMemoryStreamBuffer(Entity memoryStream) {
    auto streamData = GetMemoryStreamData(memoryStream);
    if(!streamData || !streamData->buffer) return NULL;

    return streamData->buffer->Data.data();
}

LocalFunction(OnRemoved, void, Entity component, Entity entity) {
	auto data = GetMemoryStreamData(entity);

	if(!data) return;

	if (data->buffer) {
		delete data->buffer;
		data->buffer = 0;
	}
}

LocalFunction(OnAdded, void, Entity component, Entity entity) {
	auto data = GetMemoryStreamData(entity);

	data->buffer = new MemoryBuffer();
}

BeginUnit(MemoryStream)
    BeginComponent(MemoryStream)
        RegisterBase(Stream)
    EndComponent()

    RegisterStreamProtocol(MemoryStream, "memory")

	RegisterSubscription(EventOf_EntityComponentRemoved(), OnRemoved, ComponentOf_MemoryStream())
	RegisterSubscription(EventOf_EntityComponentAdded(), OnAdded, ComponentOf_MemoryStream())
EndUnit()
