//
// Created by Kim Johannsen on 13/01/2018.
//

#include "FileStream.h"
#include "Folder.h"
#include <Foundation/Stream.h>
#include <stdio.h>
#include <Core/String.h>
#include <Core/Hierarchy.h>
#include <Core/Dictionary.h>
#include <Foundation/MemoryStream.h>

#define ProtocolIdentifier "file"

struct FileStream {
    FileStream() : fd(NULL) {}

    FILE *fd;
};

DefineComponent(FileStream)
    Dependency(Stream)
EndComponent()

DefineService(FileStream)
EndService()

static u64 Read(Entity entity, u64 size, void *data) {
    if(!GetFileStream(entity)->fd) return 0;

    u8 *bytes = (u8*)data;
    u64 remaining = size;
    while(remaining) {
        auto numRead = fread(bytes, 1, remaining, GetFileStream(entity)->fd);
        if(!numRead) {
            Log(LogChannel_Core, LogSeverity_Error, "Error reading at offset '%llu' in file '%s'", ftell(GetFileStream(entity)->fd), GetStreamPath(entity));
            return 0;
        }

        remaining -= numRead;
        bytes += numRead;
    }

    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    if(!GetFileStream(entity)->fd) return 0;
    return fwrite(data, 1, size, GetFileStream(entity)->fd);
}

static s32 Tell(Entity entity) {
    if(!GetFileStream(entity)->fd) return 0;
    return ftell(GetFileStream(entity)->fd);
}

static bool Seek(Entity entity, s32 offset) {
    if(!GetFileStream(entity)->fd) return false;

    return fseek(GetFileStream(entity)->fd, offset == StreamSeek_End ? 0 : offset, offset == StreamSeek_End ? SEEK_END : SEEK_SET) == 0;
}

static bool Open(Entity entity, int modeFlag) {
    auto data = GetFileStream(entity);
    if(data->fd) return true;

    const char *mode;
    if(modeFlag == StreamMode_Read) {
        mode = "rb";
    } else if(modeFlag == StreamMode_Write) {
        mode = "wb";
    } else if(modeFlag == (StreamMode_Read | StreamMode_Write)) {
        mode = "rb+";
    } else {
        return false;
    }

    auto nativePath = GetStreamResolvedPath(entity);
    if(memcmp(nativePath, "file://", 7) != 0) {
        Log(LogChannel_Core, LogSeverity_Error, "%s", "File stream has wrong protocol identifier.");
        return false;
    }

    nativePath += 7; // Remove 'file://'

    data->fd = fopen(nativePath, mode);

    return data->fd;
}

static void Close(Entity entity) {
    auto data = GetFileStream(entity);
    if(!data->fd) return;

    fclose(data->fd);
    data->fd = NULL;
}

static bool IsOpen(Entity entity) {
    auto data = GetFileStream(entity);
    return data->fd;
}

static bool ServiceStart() {
    StreamProtocol p {
            AddFileStream,
            RemoveFileStream,
            Seek,
            Tell,
            Read,
            Write,
            IsOpen,
            Open,
            Close
    };

    AddStreamProtocol(ProtocolIdentifier, &p);

    return true;
}

static bool ServiceStop() {
    RemoveStreamProtocol(ProtocolIdentifier);

    return true;
}
