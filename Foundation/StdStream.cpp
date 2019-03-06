//
// Created by Kim on 08-09-2018.
//

#include "StdStream.h"
#include <Foundation/Stream.h>
#include <Foundation/NativeUtils.h>

#if defined(WIN32) && defined(_MSC_VER)
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif

#include <stdio.h>
#include <stdlib.h>

struct StdStream {
};

static u64 Read(Entity entity, u64 size, void *data) {
    return fread(data, 1, size, STDIN_FILENO);
}

static u64 Write(Entity entity, u64 size, const void *data) {
    return fwrite(data, 1, size, STDIN_FILENO);
}

static s32 Tell(Entity entity) {
    return ftell(STDIN_FILENO);
}

static bool Seek(Entity entity, s32 offset) {
    return false;
}

static bool Open(Entity entity, int modeFlag) {
    return true;
}

static bool Close(Entity entity) {
    return true;
}

static bool IsOpen(Entity entity) {
    return true;
}

static bool Delete(Entity entity) {
    return false;
}

BeginUnit(StdStream)
    BeginComponent(StdStream)
        RegisterBase(Stream)
    EndComponent()

    RegisterStreamProtocol(StdStream, "std")
EndComponent()
