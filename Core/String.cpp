//
// Created by Kim Johannsen on 03/01/2018.
//

#include <Core/String.h>
#include <Core/Pool.h>

#include <string>

#define RING_MAX 2048

static char ringBuffer[RING_MAX];
static u32 ringBufferHead = 0;

template<>
const char * ApiConvert(const String& str) {
    return str.c_str();
}

StringRef FormatString(StringRef format, ...) {
    char buffer[RING_MAX];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer,RING_MAX,format, args);
    va_end (args);

    auto len = strlen(buffer);

    Assert(len < RING_MAX);

    if(ringBufferHead + len > RING_MAX) {
        ringBufferHead = 0;
    }

    char *dest = &ringBuffer[ringBufferHead];
    ringBufferHead += len + 1;

    strcpy(dest, buffer);

    return dest;
}


