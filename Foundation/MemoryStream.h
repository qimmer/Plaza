#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <Core/NativeUtils.h>

Unit(MemoryStream)
    Component(MemoryStream)

void SetMemoryStreamCapacity(Entity memoryStream, u32 capacity);
NativePtr GetMemoryStreamBuffer(Entity memoryStream);

#endif