//
// Created by Kim Johannsen on 07-03-2018.
//

#include "PerFrameStackAllocator.h"
#include "Debug.h"

#define PerFrameStackSize 1024 * 32 // (32kB)

static std::vector<char> bytes(PerFrameStackSize);

void *PerFrameStackAlloc(u32 size) {
    if(size > (bytes.capacity() - bytes.size())) {
        Log(LogChannel_Core, LogSeverity_Warning, "Per-frame stack allocator full. Reallocation performed which may cause hiccups.");
    }

    bytes.resize(bytes.size() + size);

    return bytes.data() - size;
}

void PerFrameStackReset() {
    bytes.clear();
}

void PerFrameStackReserve(u32 size) {
    bytes.reserve(size);
}

