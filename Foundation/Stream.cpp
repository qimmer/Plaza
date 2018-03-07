//
// Created by Kim Johannsen on 13/01/2018.
//

#include <Core/String.h>
#include <Core/Dictionary.h>
#include <File/Folder.h>
#include "Stream.h"
#include "Invalidation.h"
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

static Dictionary<String, StreamProtocol> Protocols;
static Dictionary<String, StreamCompressor> Compressors;
static Dictionary<String, String> FileExtensionMimeTypes;

DefineComponent(Stream)
    Dependency(Invalidation)
    DefineProperty(StringRef, StreamPath)
EndComponent()

DefineService(Stream)
EndService()

DefineComponentProperty(Stream, StringRef, StreamPath)

DefineEvent(StreamContentChanged, EntityHandler)

bool StreamSeek(Entity entity, s32 offset){
    if(!GetStream(entity)->Protocol.StreamSeekHandler) {
        return false;
    }

    return GetStream(entity)->Protocol.StreamSeekHandler(entity, offset);
}

s32 StreamTell(Entity entity){
    if(!GetStream(entity)->Protocol.StreamTellHandler) {
        return 0;
    }

    return GetStream(entity)->Protocol.StreamTellHandler(entity);
}

u64 StreamRead(Entity entity, u64 size, void *data){
    if(!GetStream(entity)->Protocol.StreamReadHandler || !(GetStream(entity)->Mode & StreamMode_Read)) {
        return 0;
    }

    return GetStream(entity)->Protocol.StreamReadHandler(entity, size, data);
}

u64 StreamWrite(Entity entity, u64 size, const void *data){
    if(!GetStream(entity)->Protocol.StreamWriteHandler || !(GetStream(entity)->Mode & StreamMode_Write)) {
        return 0;
    }

    GetStream(entity)->InvalidationPending = true;

    return GetStream(entity)->Protocol.StreamWriteHandler(entity, size, data);
}

bool IsStreamOpen(Entity entity) {
    if(!GetStream(entity)->Protocol.StreamIsOpenHandler) {
        return false;
    }

    return GetStream(entity)->Protocol.StreamIsOpenHandler(entity);
}

bool StreamOpen(Entity entity, int mode) {
    auto data = GetStream(entity);
    if(!data->Protocol.StreamOpenHandler) {
        return false;
    }

    data->Mode = mode;

    if(data->Protocol.StreamOpenHandler(entity, mode)) {
        data->Mode = mode;
        return true;
    } else {
        data->Mode = 0;
    }

    return false;
}

void StreamClose(Entity entity) {
    auto data = GetStream(entity);
    if(data->Protocol.StreamCloseHandler) {
        data->Protocol.StreamCloseHandler(entity);

        if(data->InvalidationPending) {
            data->InvalidationPending = false;
            data->Mode = 0;
            SetInvalidated(entity, true);
            FireEvent(StreamContentChanged, entity);
        }
    }
}

void StreamReadAsync(Entity entity, u64 size, void *data, StreamAsyncHandler readFinishedHandler){

}

void StreamWriteAsync(Entity entity, u64 size, const void *data, StreamAsyncHandler writeFinishedHandler){

}

void AddStreamProtocol(StringRef protocolIdentifier, struct StreamProtocol *protocol) {
    Protocols[protocolIdentifier] = *protocol;
}

void RemoveStreamProtocol(StringRef protocolIdentifier) {
    auto it = Protocols.find(protocolIdentifier);
    if(it != Protocols.end()) {
        Protocols.erase(it);
    }
}

void AddStreamCompressor(StringRef mimeType, struct StreamCompressor *compressor) {
    Compressors[mimeType] = *compressor;
}

void RemoveStreamCompressor(StringRef mimeType) {
    auto it = Compressors.find(mimeType);
    if(it != Compressors.end()) {
        Compressors.erase(it);
    }
}

StringRef GetStreamResolvedPath(Entity entity) {
    return GetStream(entity)->ResolvedPath.c_str();
}

int GetStreamMode(Entity entity) {
    return GetStream(entity)->Mode;
}

StringRef GetStreamMimeType(Entity entity) {
    auto mimeIt = FileExtensionMimeTypes.find(GetFileExtension(GetStreamResolvedPath(entity)));
    if(mimeIt != FileExtensionMimeTypes.end()) {
        return mimeIt->second.c_str();
    } else {
        return "application/octet-stream";
    }
}

void AddFileType(StringRef fileExtension, StringRef mimeType) {
    FileExtensionMimeTypes[fileExtension] = mimeType;
}

void RemoveFileType(StringRef fileExtension) {
    FileExtensionMimeTypes.erase(fileExtension);
}

u64 StreamDecompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData) {
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

u64 StreamCompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, const void *uncompressedData) {
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

static void OnStreamPathChanged(Entity entity, StringRef oldValue, StringRef newValue) {
    auto data = GetStream(entity);

    if(IsStreamOpen(entity)) {
        StreamClose(entity);
    }

    if(data->Protocol.StreamProtocolUnset) {
        data->Protocol.StreamProtocolUnset(entity);
    }

    memset(&data->Protocol, 0, sizeof(StreamProtocol));

    data->ResolvedPath = ResolveVirtualPath(newValue);

    auto resolvedPath = data->ResolvedPath.c_str();
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
    FireEvent(StreamContentChanged, entity);
}

static void OnStreamAdded(Entity entity) {
    SetStreamPath(entity, FormatString("memory://%llu.bin", entity));
}
static bool ServiceStart() {
    SubscribeStreamAdded(OnStreamAdded);
    SubscribeStreamPathChanged(OnStreamPathChanged);
}

static bool ServiceStop() {
    UnsubscribeStreamPathChanged(OnStreamPathChanged);
}
