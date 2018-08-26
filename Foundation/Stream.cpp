//
// Created by Kim Johannsen on 13/01/2018.
//
#ifdef WIN32
#include <direct.h>
#include <shlwapi.h>
#undef GetHandle
#undef CreateService
#undef CreateEvent
#endif

#include <File/Folder.h>
#include <Core/Debug.h>
#include "Stream.h"
#include "VirtualPath.h"
#include "NativeUtils.h"

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <Core/Identification.h>
#include <Core/Math.h>

#include <unistd.h>

using namespace eastl;

struct StreamExtensionModule {
    Vector(ModuleStreamProtocols, Entity, 8)
    Vector(ModuleStreamCompressors, Entity, 8)
    Vector(ModuleFileTypes, Entity, 8)
    Vector(ModuleSerializers, Entity, 8)
};

struct Stream {
    Entity StreamProtocol, StreamCompressor, StreamFileType;

    char StreamPath[PathMax];
    char StreamResolvedPath[PathMax];
    int StreamMode;

    bool InvalidationPending;
};

struct FileType {
    Entity FileTypeComponent;
    char FileTypeExtension[16];
    char FileTypeMimeType[128 - 16 - sizeof(Entity)];
};

API_EXPORT bool StreamSeek(Entity entity, s32 offset){
    auto streamData = GetStreamData(entity);
    if(!streamData) return false;

    auto protocolData = GetStreamProtocolData(streamData->StreamProtocol);
    if(!protocolData) return false;

    if(!protocolData->StreamSeekHandler) {
        return false;
    }

    return protocolData->StreamSeekHandler(entity, offset);
}

API_EXPORT s32 StreamTell(Entity entity){
    auto streamData = GetStreamData(entity);
    if(!streamData) return false;

    auto protocolData = GetStreamProtocolData(streamData->StreamProtocol);
    if(!protocolData) return false;

    if(!protocolData->StreamTellHandler) {
        return false;
    }

    return protocolData->StreamTellHandler(entity);
}

API_EXPORT u64 StreamRead(Entity entity, u64 size, void *data){
    auto streamData = GetStreamData(entity);
    if(!streamData) return false;

    auto protocolData = GetStreamProtocolData(streamData->StreamProtocol);
    if(!protocolData) return false;

    if(!protocolData->StreamReadHandler || !(streamData->StreamMode & StreamMode_Read)) {
        return 0;
    }

    return protocolData->StreamReadHandler(entity, size, data);
}

API_EXPORT u64 StreamWrite(Entity entity, u64 size, const void *data){
    auto streamData = GetStreamData(entity);
    if(!streamData) return false;

    auto protocolData = GetStreamProtocolData(streamData->StreamProtocol);
    if(!protocolData) return false;

    if(!protocolData->StreamWriteHandler || !(streamData->StreamMode & StreamMode_Write)) {
        return false;
    }

    streamData->InvalidationPending = true;

    return protocolData->StreamWriteHandler(entity, size, data);
}

API_EXPORT bool IsStreamOpen(Entity entity) {
    auto streamData = GetStreamData(entity);
    if(!streamData) return false;

    auto protocolData = GetStreamProtocolData(streamData->StreamProtocol);
    if(!protocolData) return false;

    if(!protocolData->StreamIsOpenHandler) {
        return false;
    }

    return protocolData->StreamIsOpenHandler(entity);
}

API_EXPORT bool StreamOpen(Entity entity, int mode) {
    if(!IsEntityValid(entity)) {
        return false;
    }

    auto data = GetStreamData(entity);
    if(!data) return false;

    auto protocolData = GetStreamProtocolData(data->StreamProtocol);
    if(!protocolData) return false;

    if(!protocolData->StreamOpenHandler) {
        return false;
    }

    data->StreamMode = mode;

    if(protocolData->StreamOpenHandler(entity, mode)) {
        data->StreamMode = mode;
        return true;
    } else {
        data->StreamMode = 0;
        //Log(entity, LogSeverity_Error, "Stream '%s' could not be opened.", data->StreamPath);
    }

    return false;
}

API_EXPORT bool StreamDelete(Entity entity) {
    auto data = GetStreamData(entity);
    if (!data) return false;

    auto protocolData = GetStreamProtocolData(data->StreamProtocol);
    if (!protocolData) return false;

    if (protocolData->StreamDeleteHandler) {
        return protocolData->StreamDeleteHandler(entity);
    }

    return false;
}

API_EXPORT bool StreamClose(Entity entity) {
    auto data = GetStreamData(entity);
    if(!data) return false;

    auto protocolData = GetStreamProtocolData(data->StreamProtocol);
    if(!protocolData) return false;

    if(protocolData->StreamCloseHandler) {
        protocolData->StreamCloseHandler(entity);

        if(data->InvalidationPending) {
            data->InvalidationPending = false;
            data->StreamMode = 0;

			Type types[] = { TypeOf_Entity };
			const void* values[] = { &entity };
			FireEventFast(EventOf_StreamContentChanged(), 1, types, values);
        }

        return true;
    }

    return false;
}

API_EXPORT void StreamReadAsync(Entity entity, u64 size, StreamReadAsyncHandler readFinishedHandler){
    char *buffer = (char*)malloc(size);
    StreamRead(entity, size, buffer);
    readFinishedHandler(entity, size, buffer);
    free(buffer);
}

API_EXPORT void StreamWriteAsync(Entity entity, u64 size, const void *data, StreamWriteAsyncHandler writeFinishedHandler){
    char *buffer = (char*)malloc(size);
	Assert(entity, buffer);
    memcpy(buffer, data, size);
    StreamWrite(entity, size, buffer);
    writeFinishedHandler(entity, size);
    free(buffer);
}

API_EXPORT StringRef GetStreamResolvedPath(Entity entity) {
    Assert(entity, IsEntityValid(entity));
    return GetStreamData(entity)->StreamResolvedPath;
}

API_EXPORT int GetStreamMode(Entity entity) {
    return GetStreamData(entity)->StreamMode;
}

API_EXPORT u64 StreamDecompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData) {
    auto data = GetStreamData(entity);

    auto compressorData = GetStreamCompressorData(data->StreamCompressor);
    if(!compressorData) return false;

    if(compressorData->DecompressHandler) {
        if(!compressorData->DecompressHandler(entity, uncompressedOffset, uncompressedSize, uncompressedData)) {
            return 0;
        }

        return uncompressedSize;
    }

    // no decompressor, just use raw data
    StreamSeek(entity, uncompressedOffset);
    return StreamRead(entity, uncompressedSize, uncompressedData);
}

API_EXPORT u64 StreamCompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, const void *uncompressedData) {
    auto data = GetStreamData(entity);
    auto compressorData = GetStreamCompressorData(data->StreamCompressor);
    if(!compressorData) return false;

    if(compressorData->CompressHandler) {
        if(!compressorData->CompressHandler(entity, uncompressedOffset, uncompressedSize, uncompressedData)) {
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
    auto lastSlash = strrchr(absolutePath, '/');
    if(lastSlash) absolutePath = lastSlash + 1;
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
    static char path[PathMax];
    getcwd(path, PathMax);
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

    string absolutePath = messyPath;
    if(isRelative) {
        absolutePath = GetCurrentWorkingDirectory();
        absolutePath += "/";
        absolutePath += messyPath;
    }

    replace(absolutePath.begin(), absolutePath.end(), '\\', '/');

    vector<string> pathElements;
    pathElements.clear();
    string element;
    size_t start = 0;
    while(start < absolutePath.length()) {
        auto nextSep = absolutePath.find('/', start);
        if(nextSep == string::npos) nextSep = absolutePath.length();
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

    string combined = "file://";

    for(auto i = 0; i < pathElements.size(); ++i) {
        combined.append(pathElements[i]);
        if(i < pathElements.size() - 1) {
            combined.append("/");
        }
    }

    strcpy(dest, combined.c_str());
}


API_EXPORT bool SetStreamData(Entity stream, u32 offset, u32 numBytes, const char *data) {
    if(!StreamOpen(stream, StreamMode_Write)) {
        return false;
    }

    StreamSeek(stream, offset);
    StreamWrite(stream, numBytes, data);
    StreamClose(stream);

    return true;
}


API_EXPORT bool StreamCopy(Entity source, Entity destination) {
    bool sourceWasOpen = IsStreamOpen(source);
    bool destinationWasOpen = IsStreamOpen(destination);
    if(!sourceWasOpen && !StreamOpen(source, StreamMode_Read)) return false;
    if(!destinationWasOpen && !StreamOpen(destination, StreamMode_Write)) return false;

    StreamSeek(source, StreamSeek_End);
    auto size = StreamTell(source);
    StreamSeek(source, 0);

    char buffer[4096];
    u32 numWrittenTotal = 0;
    while(numWrittenTotal < size) {
        auto numRead = StreamRead(source, Min(4096, size - numWrittenTotal), buffer);
        auto numWritten = StreamWrite(destination, numRead, buffer);

        if(numWritten < numRead || numWritten == 0) {
            if(!sourceWasOpen) StreamClose(source);
            if(!destinationWasOpen) StreamClose(destination);

            return false;
        }
    }

    if(!sourceWasOpen) StreamClose(source);
    if(!destinationWasOpen) StreamClose(destination);

    return true;
}

LocalFunction(OnStreamFileTypeChanged, void, Entity stream, Entity oldFileType, Entity newFileType) {
    if(IsEntityValid(oldFileType)) {
        auto fileTypeComponent = GetFileTypeComponent(oldFileType);

        if(IsEntityValid(fileTypeComponent)) {
            RemoveComponent(stream, fileTypeComponent);
        }
    }

    if(IsEntityValid(newFileType)) {
        auto fileTypeComponent = GetFileTypeComponent(newFileType);

        if(IsEntityValid(fileTypeComponent)) {
            AddComponent(stream, fileTypeComponent);
        }
    }
}

LocalFunction(OnStreamPathChanged, void, Entity entity, StringRef oldValue, StringRef newValue) {
    auto data = GetStreamData(entity);

    if(IsStreamOpen(entity)) {
        StreamClose(entity);
    }

    SetStreamFileType(entity, 0);
    SetStreamCompressor(entity, 0);
    SetStreamProtocol(entity, 0);

    // Resolve protocol, compressor and filetype first
    char resolvedPath[PathMax];
    ResolveVirtualPath(newValue, PathMax, resolvedPath);

    strncpy(data->StreamResolvedPath, resolvedPath, sizeof(data->StreamResolvedPath));

    char protocolIdentifier[32];
    auto colonLocation = strstr(resolvedPath, "://");

    if(!colonLocation) {
        //Log(entity, LogSeverity_Error, "Malformed URI: %s (resolved from %s)", resolvedPath, data->StreamPath);
        return;
    }

    u32 len = colonLocation - resolvedPath;
    strncpy(protocolIdentifier, resolvedPath, len);
    protocolIdentifier[len] = 0;

	auto path = GetStreamResolvedPath(entity);
    auto extension = GetFileExtension(path);

    StringRef mimeType = "application/octet-stream";
    auto oldProtocol = data->StreamProtocol;

    // Find protocol
    for_entity(protocolEntity, protocolData, StreamProtocol) {
        if(strcmp(protocolIdentifier, protocolData->StreamProtocolIdentifier) == 0) {
            SetStreamProtocol(entity, protocolEntity);
            break;
        }
    }

    if(!IsEntityValid(data->StreamProtocol)) {
        Log(entity, LogSeverity_Error, "Unknown stream protocol: %s (%s)", protocolIdentifier, resolvedPath);
        return;
    }

    if(strlen(extension) > 0) {
        // Find Filetype (optional)
        for_entity(fileTypeEntity, fileTypeData, FileType) {
            if(strcmp(extension, fileTypeData->FileTypeExtension) == 0) {
                SetStreamFileType(entity, fileTypeEntity);
                if(fileTypeData->FileTypeMimeType) {
                    mimeType = fileTypeData->FileTypeMimeType;
                    break;
                }
            }
        }

        if(!IsEntityValid(GetStreamFileType(entity))) {
            Log(entity, LogSeverity_Warning, "Unknown file type: %s", extension);
        }
    }

    // Find compressor (optional)
    for_entity(compressorEntity, compressorData, StreamCompressor) {
        if(strcmp(mimeType, compressorData->StreamCompressorMimeType) == 0) {
            SetStreamCompressor(entity, compressorEntity);
        }
    }

    // Update stream with new protocol, compressor and filetype
    if(IsEntityValid(oldProtocol)) {
        auto protocolComponent = GetStreamProtocolData(oldProtocol)->StreamProtocolComponent;
        if(IsEntityValid(protocolComponent)) {
            RemoveComponent(entity, protocolComponent);
        }
    }

    if(IsEntityValid(data->StreamProtocol)) {
        auto protocolComponent = GetStreamProtocolData(data->StreamProtocol)->StreamProtocolComponent;
        if(IsEntityValid(protocolComponent)) {
            AddComponent(entity, protocolComponent);
        }
    }

    // Find serializer (optional)
    for_entity(serializerEntity, serializerData, Serializer) {
        if(strcmp(mimeType, serializerData->SerializerMimeType) == 0) {
            AddComponent(entity, ComponentOf_PersistancePoint());
            break;
        }
    }

	Type types[] = { TypeOf_Entity };
	const void* values[] = { &entity };
	FireEventFast(EventOf_StreamContentChanged(), 1, types, values);
}

LocalFunction(OnProtocolChanged, void, Entity protocol) {
    for_entity(stream, streamData, Stream) {
        if(streamData->StreamProtocol == protocol) {
            OnStreamPathChanged(stream, streamData->StreamPath, streamData->StreamPath);
        }
    }
}

LocalFunction(OnFileTypeChanged, void, Entity fileType) {
    for_entity(stream, streamData, Stream) {
        if(streamData->StreamFileType == fileType) {
            OnStreamPathChanged(stream, streamData->StreamPath, streamData->StreamPath);
        }
    }
}

LocalFunction(OnCompressorChanged, void, Entity compressor) {
    for_entity(stream, streamData, Stream) {
        if(streamData->StreamCompressor == compressor) {
            OnStreamPathChanged(stream, streamData->StreamPath, streamData->StreamPath);
        }
    }
}

BeginUnit(Stream)
    BeginComponent(Stream)
        RegisterProperty(StringRef, StreamPath)
        RegisterProperty(Entity, StreamProtocol)
        RegisterProperty(Entity, StreamCompressor)
        RegisterProperty(Entity, StreamFileType)
    EndComponent()

    BeginComponent(StreamProtocol)
        RegisterProperty(StringRef, StreamProtocolIdentifier)
        RegisterProperty(Entity, StreamProtocolComponent)
    EndComponent()

    BeginComponent(StreamCompressor)
        RegisterProperty(StringRef, StreamCompressorMimeType)
    EndComponent()

    BeginComponent(FileType)
        RegisterProperty(StringRef, FileTypeExtension)
        RegisterProperty(StringRef, FileTypeMimeType)
        RegisterProperty(Entity, FileTypeComponent)
    EndComponent()

    BeginComponent(Serializer)
        RegisterProperty(StringRef, SerializerMimeType)
    EndComponent()

    BeginComponent(StreamExtensionModule)
        RegisterBase(Module)
        RegisterArrayProperty(StreamProtocol, ModuleStreamProtocols)
        RegisterArrayProperty(StreamCompressor, ModuleStreamCompressors)
        RegisterArrayProperty(FileType, ModuleFileTypes)
        RegisterArrayProperty(Serializer, ModuleSerializers)
    EndComponent()

    RegisterEvent(StreamContentChanged)

    RegisterSubscription(StreamPathChanged, OnStreamPathChanged, 0)
    RegisterSubscription(StreamProtocolIdentifierChanged, OnProtocolChanged, 0)
    RegisterSubscription(StreamProtocolComponentChanged, OnProtocolChanged, 0)
    RegisterSubscription(FileTypeExtensionChanged, OnFileTypeChanged, 0)
    RegisterSubscription(FileTypeMimeTypeChanged, OnFileTypeChanged, 0)
    RegisterSubscription(StreamCompressorMimeTypeChanged, OnCompressorChanged, 0)
    RegisterSubscription(StreamFileTypeChanged, OnStreamFileTypeChanged, 0)
EndUnit()


