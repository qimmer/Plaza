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
#include <Foundation/AppLoop.h>
#include "FileWatcher/FileWatcher.h"

#define ProtocolIdentifier "file"

struct FileStream {
    FileStream() : fd(NULL) {}

    FILE *fd;
    FW::WatchID watchID;
};

DefineComponent(FileStream)
    Dependency(Stream)
EndComponent()

static FW::FileWatcher fileWatcher;

class UpdateListener : public FW::FileWatchListener
{
public:
    UpdateListener() {}
    void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename,
                          FW::Action action)
    {
        if(action == FW::Actions::Modified) {
            for_entity(fileStream, FileStream) {
                auto data = GetFileStream(fileStream);
                if(data->watchID == watchid) {
                    FireEvent(StreamContentChanged, fileStream);
                    break;
                }
            }
        }
    }
};

static UpdateListener listener;

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
    if(strlen(nativePath) < 7 || memcmp(nativePath, "file://", 7) != 0) {
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

static void OnServiceStart(Service service) {
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
}

static void OnServiceStop(Service service){
    RemoveStreamProtocol(ProtocolIdentifier);
}

static void OnStreamPathChanged(Entity stream, StringRef oldPath, StringRef newPath) {
    if(HasFileStream(stream)) {
        auto data = GetFileStream(stream);
        fileWatcher.removeWatch(data->watchID);

        auto nativePath = GetStreamResolvedPath(stream);
        if(strlen(nativePath) < 7 || memcmp(nativePath, "file://", 7) != 0) {
            return;
        }

        nativePath += 7; // Remove 'file://'

        char parentFolder[PATH_MAX];
        GetParentFolder(nativePath, parentFolder, PATH_MAX);
        data->watchID = fileWatcher.addWatch(parentFolder, &listener, false);
    }
}

static void OnFileStreamAdded(Entity stream) {
    OnStreamPathChanged(stream, "", GetStreamResolvedPath(stream));
}

static void OnFileStreamRemoved(Entity stream) {
    auto data = GetFileStream(stream);
    fileWatcher.removeWatch(data->watchID);
}

static void OnAppUpdate(double deltaTime) {
    static double timeSinceUpdate = 0.0;
    timeSinceUpdate += deltaTime;

    if(timeSinceUpdate > 1.0) {
        timeSinceUpdate = 0.0;

        fileWatcher.update();
    }
}

DefineService(FileStream)
    Subscribe(AppUpdate, OnAppUpdate)
    Subscribe(StreamPathChanged, OnStreamPathChanged)
    Subscribe(FileStreamAdded, OnFileStreamAdded)
    Subscribe(FileStreamRemoved, OnFileStreamRemoved)
    Subscribe(FileStreamStarted, OnServiceStart)
    Subscribe(FileStreamStopped, OnServiceStop)
EndService()

StringRef GetNativePath(StringRef resourcePath) {
    auto fileIdentifier = strstr(resourcePath, "file://");
    if(fileIdentifier) {
        return fileIdentifier + 7;
    }

    Log(LogChannel_Core, LogSeverity_Error, "Resource path is not a file path: %s", resourcePath);
    return 0;
}
