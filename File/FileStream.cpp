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
#include <Core/Date.h>
#include "FileWatcher/FileWatcher.h"

struct FileStream {
    FILE *fd;
    FW::WatchID watchID;
};

static FW::FileWatcher fileWatcher;

static void OnHandleFileChanged(Entity fileStream, StringRef path, u32 pathLength, FW::WatchID watchid) {
    auto data = GetFileStream(fileStream);
    auto streamData = GetStream(fileStream);
    auto resolvedPathLength = strlen(streamData.StreamResolvedPath);

    if(data.watchID == watchid && resolvedPathLength == pathLength && !stricmp(path, streamData.StreamResolvedPath)) {
        streamData.StreamLastWrite = GetDateNow();
        SetStream(fileStream, streamData);
    }
}

class UpdateListener : public FW::FileWatchListener
{
public:
    UpdateListener() {}
    void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename,
                          FW::Action action)
    {
        if(action == FW::Actions::Modified) {
            auto pathLength = dir.length() + filename.length() + strlen("file://") + 1;
            char *path = (char*)alloca(pathLength + 1);
            sprintf(path, "file://%s/%s", dir.c_str(), filename.c_str());

            for_entity(fileStream, ComponentOf_FileStream()) {
                OnHandleFileChanged(fileStream, path, pathLength, watchid);
            }
        }
    }
};

static UpdateListener listener;

static u64 Read(Entity entity, u64 size, void *data) {
    if(!GetFileStream(entity).fd) return 0;

    u8 *bytes = (u8*)data;
    u64 remaining = size;
    while(remaining) {
        auto numRead = fread(bytes, 1, remaining, GetFileStream(entity).fd);
        if(!numRead) {
            Log(entity, LogSeverity_Error, "Error reading at offset '%llu' in file '%s'", ftell(GetFileStream(entity).fd), GetStream(entity).StreamPath);
            return 0;
        }

        remaining -= numRead;
        bytes += numRead;
    }

    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    if(!GetFileStream(entity).fd) return 0;
    return fwrite(data, 1, size, GetFileStream(entity).fd);
}

static s32 Tell(Entity entity) {
    if(!GetFileStream(entity).fd) return 0;
    return ftell(GetFileStream(entity).fd);
}

static bool Seek(Entity entity, s32 offset) {
    if(!GetFileStream(entity).fd) return false;

    auto origin = offset == StreamSeek_End ? SEEK_END : SEEK_SET;
    offset = offset == StreamSeek_End ? 0 : offset;

    return fseek(GetFileStream(entity).fd, offset, origin) == 0;
}

static bool Open(Entity entity, int modeFlag) {
    auto data = GetFileStream(entity);
    if(data.fd) return true;

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

    auto nativePath = GetStream(entity).StreamResolvedPath;
    if(strlen(nativePath) < 7 || memcmp(nativePath, "file://", 7) != 0) {
        Log(entity, LogSeverity_Error, "%s", "File stream has wrong protocol identifier.");
        return false;
    }

    nativePath += 7; // Remove 'file://'

    data.fd = fopen(nativePath, mode);
    SetFileStream(entity, data);

    return data.fd;
}

static bool Close(Entity entity) {
    auto data = GetFileStream(entity);
    if(!data.fd) return false;

    data.fd = NULL;

    SetFileStream(entity, data);

    return true;
}

static bool IsOpen(Entity entity) {
    auto data = GetFileStream(entity);
    return data.fd;
}

static bool Delete(Entity entity) {
    auto nativePath = GetStream(entity).StreamResolvedPath;
    if(strlen(nativePath) < 7 || memcmp(nativePath, "file://", 7) != 0) {
        Log(entity, LogSeverity_Error, "%s", "File stream has wrong protocol identifier.");
        return false;
    }

    nativePath += 7; // Remove 'file://'

    return remove(nativePath) == 0;
}

static void OnStreamChanged(Entity stream, const Stream& oldData, const Stream& newData) {
    if((oldData.StreamProtocol != ProtocolOf_File() && newData.StreamProtocol == ProtocolOf_File()) ||
       (oldData.StreamResolvedPath != newData.StreamResolvedPath && newData.StreamProtocol == ProtocolOf_File())) {

        auto data = GetFileStream(stream);
        if(data.watchID) {
            fileWatcher.removeWatch(data.watchID);
            data.watchID = 0;
        }

        auto nativePath = GetStream(stream).StreamResolvedPath;
        if(strlen(nativePath) < 7 || memcmp(nativePath, "file://", 7) != 0) {
            return;
        }

        nativePath += 7; // Remove 'file://'

        auto parentFolder = GetParentFolder(nativePath);
        data.watchID = fileWatcher.addWatch(parentFolder, &listener, false);
        SetFileStream(stream, data);
    }
}

static void OnFileStreamChanged(Entity stream, const FileStream& oldData, const FileStream& newData) {
    if(oldData.fd && !newData.fd) {
        fclose(oldData.fd);
    }

    if(oldData.watchID && !newData.watchID) {
        fileWatcher.removeWatch(oldData.watchID);
    }
}

static void OnUpdateFileWatchers(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
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

    RegisterStreamProtocol(File, "file")

    RegisterDeferredSystem(OnUpdateFileWatchers, ComponentOf_AppLoop(), AppLoopOrder_Input)
    RegisterSystem(OnFileStreamChanged, ComponentOf_FileStream())
    RegisterSystem(OnStreamChanged, ComponentOf_Stream())
EndComponent()
