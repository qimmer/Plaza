//
// Created by Kim Johannsen on 13/01/2018.
//

#include "FileStream.h"
#include "Folder.h"
#include <Foundation/Stream.h>
#include <stdio.h>
#include <Foundation/MemoryStream.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include <Foundation/Timer.h>
#include <Foundation/NativeUtils.h>
#include <Core/Identification.h>
#include "FileWatcher/FileWatcher.h"

#define ProtocolIdentifier "file"

struct FileStream {
    FILE *fd;
    FW::WatchID watchID;
};

static FW::FileWatcher fileWatcher;

class UpdateListener : public FW::FileWatchListener
{
public:
    UpdateListener() {}
    void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename,
                          FW::Action action)
    {
        if(action == FW::Actions::Modified) {
            for_entity(fileStream, data, FileStream) {
                auto data = GetFileStreamData(fileStream);
                if(data->watchID == watchid) {
					Type types[] = { TypeOf_Entity };
					const void* values[] = { &fileStream };
					FireEventFast(EventOf_StreamContentChanged(), 1, types, values);
                    break;
                }
            }
        }
    }
};

static UpdateListener listener;

static u64 Read(Entity entity, u64 size, void *data) {
    if(!GetFileStreamData(entity)->fd) return 0;

    u8 *bytes = (u8*)data;
    u64 remaining = size;
    while(remaining) {
        auto numRead = fread(bytes, 1, remaining, GetFileStreamData(entity)->fd);
        if(!numRead) {
            Log(entity, LogSeverity_Error, "Error reading at offset '%llu' in file '%s'", ftell(GetFileStreamData(entity)->fd), GetStreamPath(entity));
            return 0;
        }

        remaining -= numRead;
        bytes += numRead;
    }

    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    if(!GetFileStreamData(entity)->fd) return 0;
    return fwrite(data, 1, size, GetFileStreamData(entity)->fd);
}

static s32 Tell(Entity entity) {
    if(!GetFileStreamData(entity)->fd) return 0;
    return ftell(GetFileStreamData(entity)->fd);
}

static bool Seek(Entity entity, s32 offset) {
    if(!GetFileStreamData(entity)->fd) return false;

    auto origin = offset == StreamSeek_End ? SEEK_END : SEEK_SET;
    offset = offset == StreamSeek_End ? 0 : offset;

    return fseek(GetFileStreamData(entity)->fd, offset, origin) == 0;
}

static bool Open(Entity entity, int modeFlag) {
    auto data = GetFileStreamData(entity);
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
    if(strlen(nativePath) < 7 || memcmp(nativePath, "file://", 7) != 0) {
        Log(entity, LogSeverity_Error, "%s", "File stream has wrong protocol identifier.");
        return false;
    }

    nativePath += 7; // Remove 'file://'

    data->fd = fopen(nativePath, mode);

    return data->fd;
}

static bool Close(Entity entity) {
    auto data = GetFileStreamData(entity);
    if(!data->fd) return false;

    fclose(data->fd);
    data->fd = NULL;

    return true;
}

static bool IsOpen(Entity entity) {
    auto data = GetFileStreamData(entity);
    return data->fd;
}

static bool Delete(Entity entity) {
    auto nativePath = GetStreamResolvedPath(entity);
    if(strlen(nativePath) < 7 || memcmp(nativePath, "file://", 7) != 0) {
        Log(entity, LogSeverity_Error, "%s", "File stream has wrong protocol identifier.");
        return false;
    }

    nativePath += 7; // Remove 'file://'

    return remove(nativePath) == 0;
}

LocalFunction(OnStreamPathChanged, void, Entity stream, StringRef oldPath, StringRef newPath) {
    if(HasComponent(stream, ComponentOf_FileStream())) {
        auto data = GetFileStreamData(stream);
        fileWatcher.removeWatch(data->watchID);

        auto nativePath = GetStreamResolvedPath(stream);
        if(strlen(nativePath) < 7 || memcmp(nativePath, "file://", 7) != 0) {
            return;
        }

        nativePath += 7; // Remove 'file://'

        char parentFolder[PathMax];
        GetParentFolder(nativePath, parentFolder, PathMax);
        data->watchID = fileWatcher.addWatch(parentFolder, &listener, false);
    }
}

LocalFunction(OnFileStreamAdded, void, Entity component, Entity stream) {
    OnStreamPathChanged(stream, "", GetStreamResolvedPath(stream));
}

LocalFunction(OnFileStreamRemoved, void, Entity component, Entity stream) {
    auto data = GetFileStreamData(stream);
    fileWatcher.removeWatch(data->watchID);
}

LocalFunction(OnFileWatcherUpdate, void, Entity timer) {
    fileWatcher.update();
}

StringRef GetNativePath(StringRef resourcePath) {
    auto fileIdentifier = strstr(resourcePath, "file://");
    if(fileIdentifier) {
        return fileIdentifier + 7;
    }

    Log(0, LogSeverity_Error, "Resource path is not a file path: %s", resourcePath);
    return 0;
}

BeginUnit(FileStream)
    BeginComponent(FileStream)
        RegisterBase(Stream)
    EndComponent()

    RegisterSubscription(StreamPathChanged, OnStreamPathChanged, 0)
    RegisterSubscription(EntityComponentAdded, OnFileStreamAdded, ComponentOf_FileStream())
    RegisterSubscription(EntityComponentRemoved, OnFileStreamRemoved, ComponentOf_FileStream())

    RegisterStreamProtocol(FileStream, "file")
EndComponent()
