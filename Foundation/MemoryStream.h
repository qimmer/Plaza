#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <Core/NativeUtils.h>
#include <Foundation/NativeUtils.h>

struct MemoryStream {
    struct MemoryBuffer* buffer;
    s32 Offset;
    bool IsOpen;
};

Unit(MemoryStream)
    Component(MemoryStream)
    StreamProtocol(Memory)

void SetMemoryStreamCapacity(Entity memoryStream, u32 capacity);
NativePtr GetMemoryStreamBuffer(Entity memoryStream);

#endif