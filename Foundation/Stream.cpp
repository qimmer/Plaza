//
// Created by Kim Johannsen on 13/01/2018.
//
#include <direct.h>

#ifdef WIN32
#include <shlwapi.h>
#undef GetHandle
#undef CreateService
#undef CreateEvent
#endif

#include <Core/String.h>
#include <Core/Dictionary.h>
#include <File/Folder.h>
#include "Stream.h"
#include "VirtualPath.h"

struct Stream {
    Stream() :
        InvalidationPending(false),
        Protocol({NULL, NULL, NULL, NULL, NULL, NULL, NULL}) {}

    StreamProtocol Protocol;
    StreamCompressor Compressor;
    String StreamPath, ResolvedPath, MimeType;
    int Mode;

    bool InvalidationPending;
};

static Dictionary<StreamProtocol> Protocols;
static Dictionary<StreamCompressor> Compressors;
static Dictionary<String> FileExtensionMimeTypes;

DefineComponent(Stream)
    DefinePropertyReactive(StringRef, StreamPath)
EndComponent()

DefineComponentPropertyReactive(Stream, StringRef, StreamPath)

DefineEvent(StreamContentChanged)

API_EXPORT bool StreamSeek(Entity entity, s32 offset){
    if(!GetStream(entity)->Protocol.StreamSeekHandler) {
        return false;
    }

    return GetStream(entity)->Protocol.StreamSeekHandler(entity, offset);
}

API_EXPORT s32 StreamTell(Entity entity){
    if(!GetStream(entity)->Protocol.StreamTellHandler) {
        return 0;
    }

    return GetStream(entity)->Protocol.StreamTellHandler(entity);
}

API_EXPORT u64 StreamRead(Entity entity, u64 size, void *data){
    if(!GetStream(entity)->Protocol.StreamReadHandler || !(GetStream(entity)->Mode & StreamMode_Read)) {
        return 0;
    }

    return GetStream(entity)->Protocol.StreamReadHandler(entity, size, data);
}

API_EXPORT u64 StreamWrite(Entity entity, u64 size, const void *data){
    if(!GetStream(entity)->Protocol.StreamWriteHandler || !(GetStream(entity)->Mode & StreamMode_Write)) {
        return 0;
    }

    GetStream(entity)->InvalidationPending = true;

    return GetStream(entity)->Protocol.StreamWriteHandler(entity, size, data);
}

API_EXPORT bool IsStreamOpen(Entity entity) {
    if(!GetStream(entity)->Protocol.StreamIsOpenHandler) {
        return false;
    }

    return GetStream(entity)->Protocol.StreamIsOpenHandler(entity);
}

API_EXPORT bool StreamOpen(Entity entity, int mode) {
    Assert(IsEntityValid(entity));

    auto data = GetStream(entity);
    if(!data->Protocol.StreamOpenHandler) {
        Log(LogChannel_Core, LogSeverity_Error, "Steam '%s' has no open handler.", GetEntityPath(entity));
        return false;
    }

    data->Mode = mode;

    if(data->Protocol.StreamOpenHandler(entity, mode)) {
        data->Mode = mode;
        return true;
    } else {
        data->Mode = 0;
        Log(LogChannel_Core, LogSeverity_Error, "Steam '%s' could not be opened.", GetEntityPath(entity));
    }

    return false;
}

API_EXPORT void StreamClose(Entity entity) {
    auto data = GetStream(entity);
    if(data->Protocol.StreamCloseHandler) {
        data->Protocol.StreamCloseHandler(entity);

        if(data->InvalidationPending) {
            data->InvalidationPending = false;
            data->Mode = 0;
            FireNativeEvent(StreamContentChanged, entity);
        }
    }
}

API_EXPORT void StreamReadAsync(Entity entity, u64 size, StreamReadAsyncHandler readFinishedHandler){
    char *buffer = (char*)malloc(size);
    StreamRead(entity, size, buffer);
    readFinishedHandler(entity, size, buffer);
    free(buffer);
}

API_EXPORT void StreamWriteAsync(Entity entity, u64 size, const void *data, StreamWriteAsyncHandler writeFinishedHandler){
    char *buffer = (char*)malloc(size);
	Assert(buffer);
    memcpy(buffer, data, size);
    StreamWrite(entity, size, buffer);
    writeFinishedHandler(entity, size);
    free(buffer);
}

API_EXPORT void AddStreamProtocol(StringRef protocolIdentifier, struct StreamProtocol *protocol) {
    Protocols[protocolIdentifier] = *protocol;
}

API_EXPORT void RemoveStreamProtocol(StringRef protocolIdentifier) {
    auto it = Protocols.find(protocolIdentifier);
    if(it != Protocols.end()) {
        Protocols.erase(it);
    }
}

API_EXPORT void AddStreamCompressor(StringRef mimeType, struct StreamCompressor *compressor) {
    Compressors[mimeType] = *compressor;
}

API_EXPORT void RemoveStreamCompressor(StringRef mimeType) {
    auto it = Compressors.find(mimeType);
    if(it != Compressors.end()) {
        Compressors.erase(it);
    }
}

API_EXPORT StringRef GetStreamResolvedPath(Entity entity) {
    Assert(IsEntityValid(entity));
    return GetStream(entity)->ResolvedPath.c_str();
}

API_EXPORT int GetStreamMode(Entity entity) {
    return GetStream(entity)->Mode;
}

API_EXPORT StringRef GetStreamMimeType(Entity entity) {
    auto mimeIt = FileExtensionMimeTypes.find(GetFileExtension(GetStreamResolvedPath(entity)));
    if(mimeIt != FileExtensionMimeTypes.end()) {
        return mimeIt->second.c_str();
    } else {
        return "application/octet-stream";
    }
}

API_EXPORT void AddFileType(StringRef fileExtension, StringRef mimeType) {
    FileExtensionMimeTypes[fileExtension] = mimeType;
}

API_EXPORT void RemoveFileType(StringRef fileExtension) {
    FileExtensionMimeTypes.erase(fileExtension);
}

API_EXPORT u64 StreamDecompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData) {
    auto data = GetStream(entity);
    if(data->Compressor.DecompressHandler) {
        if(!data->Compressor.DecompressHandler(entity, uncompressedOffset, uncompressedSize, uncompressedData)) {
            return 0;
        }

        return uncompressedSize;
    }

    // no decompressor, just use raw data
    StreamSeek(entity, uncompressedOffset);
    return StreamRead(entity, uncompressedSize, uncompressedData);
}

API_EXPORT u64 StreamCompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, const void *uncompressedData) {
    auto data = GetStream(entity);
    if(data->Compressor.CompressHandler) {
        if(!data->Compressor.CompressHandler(entity, uncompressedOffset, uncompressedSize, uncompressedData)) {
            return 0;
        }

        return uncompressedSize;
    }

    // no decompressor, just use raw data
    StreamSeek(entity, uncompressedOffset);
    return StreamWrite(entity, uncompressedSize, uncompressedData);
}


API_EXPORT StringRef GetFileName(StringRef absolutePath) {
    auto fileName = strrchr(absolutePath, '/');
    if(!fileName) return absolutePath;
    return fileName + 1;
}

API_EXPORT StringRef GetFileExtension(StringRef absolutePath) {
    auto extension = strrchr(absolutePath, '.');
    if(!extension) return "";
    return extension;
}

API_EXPORT void GetParentFolder(StringRef absolutePath, char *parentFolder, size_t bufMax) {
    strncpy(parentFolder, absolutePath, bufMax);
    auto ptr = strrchr(parentFolder, '/');
    if(!ptr) {
        *parentFolder = 0;
        return;
    }

    parentFolder[ptr - parentFolder] = '\0';
}

API_EXPORT StringRef GetCurrentWorkingDirectory() {
    static char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    return path;
}

API_EXPORT void CleanupPath(char* messyPath) {
    char *dest = messyPath;
    auto protocolLocation = strstr(messyPath, "://");

    if(protocolLocation) {
        messyPath = protocolLocation + 3;
    }

#ifdef WIN32
    auto isRelative = ::PathIsRelativeA(messyPath) || messyPath[0] == '/';
#else
    auto isRelative = messyPath[0] != '/';
#endif

    String absolutePath = messyPath;
    if(isRelative) {
        absolutePath = GetCurrentWorkingDirectory();
        absolutePath += "/";
        absolutePath += messyPath;
    }

    std::replace(absolutePath.begin(), absolutePath.end(), '\\', '/');

    Vector<String> pathElements;
    pathElements.clear();
    String element;
    size_t start = 0;
    while(start < absolutePath.length()) {
        auto nextSep = absolutePath.find('/', start);
        if(nextSep == String::npos) nextSep = absolutePath.length();
        auto element = absolutePath.substr(start, nextSep - start);
        start = nextSep + 1;

        if(element.length() == 0) {
            // nothing in this element
            continue;
        } else if(element.length() > 1 && element.compare(element.length() - 2, 2, "..") == 0) {
            // parent directory identifier
            pathElements.pop_back();
        } else if(element.compare(element.length() - 1, 1, ".") == 0) {
            // current directory identifier
            continue;
        } else {
            pathElements.push_back(element);
        }
    }

    String combined = "file://";

#ifndef WIN32
    os << "/";
#endif

    for(auto i = 0; i < pathElements.size(); ++i) {
        combined.append(pathElements[i]);
        if(i < pathElements.size() - 1) {
            combined.append("/");
        }
    }

    strcpy(dest, combined.c_str());
}

static void OnStreamPathChanged(Entity entity, StringRef oldValue, StringRef newValue) {
    auto data = GetStream(entity);

    if(IsStreamOpen(entity)) {
        StreamClose(entity);
    }

    if(data->Protocol.StreamProtocolUnset) {
        data->Protocol.StreamProtocolUnset(entity);
    }

    memset(&data->Protocol, 0, sizeof(StreamProtocol));

    char resolvedPath[PATH_MAX];
    ResolveVirtualPath(newValue, resolvedPath);

    data->ResolvedPath = resolvedPath;

    char protocolIdentifier[32];
    auto colonLocation = strstr(resolvedPath, "://");

    if(!colonLocation) {
        Log(LogChannel_Core, LogSeverity_Error, "Malformed URI: %s (resolved from %s)", resolvedPath, data->StreamPath.c_str());
        return;
    }

    u32 len = colonLocation - resolvedPath;
    strncpy(protocolIdentifier, resolvedPath, len);
    protocolIdentifier[len] = 0;

    auto protocolIt = Protocols.find(protocolIdentifier);
    if(protocolIt == Protocols.end()) {
        Log(LogChannel_Core, LogSeverity_Error, "Unknown stream protocol: %s (%s)", protocolIdentifier, resolvedPath);
        return;
    }

    auto mimeType = GetStreamMimeType(entity);
    auto compressorIt = Compressors.find(mimeType);
    if(compressorIt != Compressors.end()) {
        data->Compressor = compressorIt->second;
    } else {
        memset(&data->Compressor, 0, sizeof(StreamCompressor));
    }

    data->Protocol = protocolIt->second;

    if(data->Protocol.StreamProtocolSet) {
        data->Protocol.StreamProtocolSet(entity);
    }
    FireNativeEvent(StreamContentChanged, entity);
}

static void OnStreamAdded(Entity entity) {
    char path[PATH_MAX];
    sprintf(path, "memory://%llu.bin", entity);
    SetStreamPath(entity, path);
}

DefineService(Stream)
        Subscribe(StreamAdded, OnStreamAdded)
        Subscribe(StreamPathChanged, OnStreamPathChanged)
EndService()
