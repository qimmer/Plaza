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

struct FileStream {
    FileStream() : FileMode(FileMode_Read), fd(NULL) {}

    String FilePath;
    int FileMode;
    FILE *fd;
};

static Dictionary<String, String> fileTypes;

DefineComponent(FileStream)
        Dependency(Stream)
        DefineProperty(StringRef, FilePath)
EndComponent()

DefineService(FileStream)
EndService()

DefineComponentProperty(FileStream, StringRef, FilePath)
DefineComponentProperty(FileStream, int, FileMode)

void RegisterFileType(StringRef fileExtension, StringRef mimeType) {
    fileTypes[fileExtension] = mimeType;
}

static u64 Read(Entity entity, u64 size, void *data) {
    if(!GetFileStream(entity)->fd) return 0;
    return fread(data, 1, size, GetFileStream(entity)->fd);
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

    return fseek(GetFileStream(entity)->fd, offset == STREAM_SEEK_END ? 0 : offset, offset == STREAM_SEEK_END ? SEEK_END : SEEK_SET) == 0;
}

static bool Open(Entity entity) {
    auto data = GetFileStream(entity);
    if(data->fd) return true;

    const char *mode;
    if(data->FileMode == FileMode_Read) {
        mode = "rb";
    } else if(data->FileMode == FileMode_Write) {
        mode = "wb";
    } else if(data->FileMode == (FileMode_Read | FileMode_Write)) {
        mode = "rb+";
    } else {
        return false;
    }

    data->fd = fopen(data->FilePath.c_str(), mode);

    return true;
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

static void OnFileStreamAdded(Entity entity) {
    SetStreamReadHandler(entity, Read);
    SetStreamWriteHandler(entity, Write);
    SetStreamSeekHandler(entity, Seek);
    SetStreamTellHandler(entity, Tell);
    SetStreamOpenHandler(entity, Open);
    SetStreamCloseHandler(entity, Close);
    SetStreamIsOpenHandler(entity, IsOpen);
}

static void OnFileStreamRemoved(Entity entity) {
    SetStreamReadHandler(entity, NULL);
    SetStreamWriteHandler(entity, NULL);
    SetStreamSeekHandler(entity, NULL);
    SetStreamTellHandler(entity, NULL);
    SetStreamOpenHandler(entity, NULL);
    SetStreamCloseHandler(entity, NULL);
    SetStreamIsOpenHandler(entity, NULL);
}

static void OnFilePathChanged(Entity entity, StringRef oldValue, StringRef newValue) {
    if(memcmp(GetName(entity), "Entity_", 7) == 0 || strcmp(oldValue, GetName(entity)) == 0) {
        SetName(entity, newValue);

        // Eventually set mime type from file extension
        auto mimeIt = fileTypes.find(GetFileExtension(newValue));
        if(mimeIt != fileTypes.end()) {
            SetStreamMimeType(entity, mimeIt->second.c_str());
        } else {
            SetStreamMimeType(entity, "application/octet-stream");
        }
    }
}

static bool ServiceStart() {
    SubscribeFileStreamAdded(OnFileStreamAdded);
    SubscribeFileStreamRemoved(OnFileStreamRemoved);
    SubscribeFilePathChanged(OnFilePathChanged);

    RegisterFileType(".txt", "text/plain");

    RegisterFileType(".txt", "text/plain");
    RegisterFileType(".txt", "text/plain");
    return true;
}

static bool ServiceStop() {
    UnsubscribeFileStreamAdded(OnFileStreamAdded);
    UnsubscribeFileStreamRemoved(OnFileStreamRemoved);
    UnsubscribeFilePathChanged(OnFilePathChanged);

    return true;
}
