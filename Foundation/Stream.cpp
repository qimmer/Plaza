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
#include <Core/Algorithms.h>
#include <Core/Date.h>

#if defined(WIN32) && defined(EA_COMPILER_MSVC)
#include <unistd.h>
#endif

using namespace eastl;

API_EXPORT bool StreamSeek(Entity entity, s32 offset){
    auto streamData = GetStream(entity);
    

    auto protocolData = GetStreamProtocol(streamData.StreamProtocol);
    

    if(!protocolData.StreamSeekHandler) {
        return false;
    }

    return protocolData.StreamSeekHandler(entity, offset);
}

API_EXPORT s32 StreamTell(Entity entity){
    auto streamData = GetStream(entity);

    auto protocolData = GetStreamProtocol(streamData.StreamProtocol);
    

    if(!protocolData.StreamTellHandler) {
        return false;
    }

    return protocolData.StreamTellHandler(entity);
}

API_EXPORT u64 StreamRead(Entity entity, u64 size, void *data){
    auto streamData = GetStream(entity);
    auto protocolData = GetStreamProtocol(streamData.StreamProtocol);

    if(!protocolData.StreamReadHandler || !(streamData.StreamMode & StreamMode_Read)) {
        return 0;
    }

    return protocolData.StreamReadHandler(entity, size, data);
}

API_EXPORT u64 StreamWrite(Entity entity, u64 size, const void *data){
    auto streamData = GetStream(entity);

    auto protocolData = GetStreamProtocol(streamData.StreamProtocol);

    if(!protocolData.StreamWriteHandler || !(streamData.StreamMode & StreamMode_Write)) {
        return 0;
    }

    streamData.InvalidationPending = true;
    SetStream(entity, streamData);

    return protocolData.StreamWriteHandler(entity, size, data);
}

API_EXPORT bool IsStreamOpen(Entity entity) {
    auto streamData = GetStream(entity);

    auto protocolData = GetStreamProtocol(streamData.StreamProtocol);

    if(!protocolData.StreamIsOpenHandler) {
        return false;
    }

    return protocolData.StreamIsOpenHandler(entity);
}

API_EXPORT bool StreamOpen(Entity entity, int mode) {
    if(!IsEntityValid(entity)) {
        return false;
    }

    auto data = GetStream(entity);

    auto protocolData = GetStreamProtocol(data.StreamProtocol);
    
    if(!protocolData.StreamOpenHandler) {
        return false;
    }

    data.StreamMode = mode;

    if(protocolData.StreamOpenHandler(entity, mode)) {
        data.StreamMode = mode;
        SetStream(entity, data);
        return true;
    } else {
        data.StreamMode = 0;
        SetStream(entity, data);
        //Log(entity, LogSeverity_Error, "Stream '%s' could not be opened.", data.StreamPath);
    }

    return false;
}

API_EXPORT bool StreamDelete(Entity entity) {
    auto data = GetStream(entity);

    auto protocolData = GetStreamProtocol(data.StreamProtocol);

    if (protocolData.StreamDeleteHandler) {
        return protocolData.StreamDeleteHandler(entity);
    }

    return false;
}

API_EXPORT bool StreamClose(Entity entity) {
    auto data = GetStream(entity);
    auto protocolData = GetStreamProtocol(data.StreamProtocol);

    if(protocolData.StreamCloseHandler) {
        protocolData.StreamCloseHandler(entity);

        if(data.InvalidationPending) {
            data.InvalidationPending = false;
            data.StreamMode = 0;
            data.StreamLastWrite = GetDateNow();

            SetStream(entity, data);
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

API_EXPORT int GetStreamMode(Entity entity) {
    if(!HasComponent(entity, ComponentOf_Stream())) return StreamMode_Closed;
    return GetStream(entity).StreamMode;
}

API_EXPORT u64 StreamDecompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData) {
    auto data = GetStream(entity);

    if(!data.StreamCompressor) {
        Error(entity, "Could not decompress data in stream. No compatible decompressor found for file type.");
        return false;
    }

    auto compressorData = GetStreamCompressor(data.StreamCompressor);

    if(compressorData.DecompressHandler) {
        if(!compressorData.DecompressHandler(entity, uncompressedOffset, uncompressedSize, uncompressedData)) {
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

    if(!data.StreamCompressor) {
        Error(entity, "Could not compress data in stream. No compatible compressor found for file type.");
    }
    auto compressorData = GetStreamCompressor(data.StreamCompressor);

    if(compressorData.CompressHandler) {
        if(!compressorData.CompressHandler(entity, uncompressedOffset, uncompressedSize, uncompressedData)) {
            return 0;
        }

        return uncompressedSize;
    }

    // no decompressor, just use raw data
    StreamSeek(entity, uncompressedOffset);
    return StreamWrite(entity, uncompressedSize, uncompressedData);
}


API_EXPORT StringRef GetFileName(StringRef absolutePath) {
    if(!absolutePath) return NULL;

    auto fileName = strrchr(absolutePath, '/');
    if(!fileName) return absolutePath;
    return Intern(fileName + 1);
}

API_EXPORT StringRef GetFileExtension(StringRef absolutePath) {
    if(!absolutePath) return NULL;

    auto lastSlash = strrchr(absolutePath, '/');
    if(lastSlash) absolutePath = lastSlash + 1;
    auto extension = strrchr(absolutePath, '.');
    if(!extension) return "";
    return Intern(extension);
}

API_EXPORT StringRef GetParentFolder(StringRef absolutePath) {
    if(!absolutePath) return NULL;
    auto buffer = (char*)alloca(strlen(absolutePath) + 1);
    strcpy(buffer, absolutePath);
    auto ptr = strrchr(buffer, '/');
    if(!ptr) {
        return NULL;
    }

    buffer[ptr - buffer] = '\0';

    return Intern(buffer);
}

API_EXPORT StringRef GetCurrentWorkingDirectory() {
    static char path[PathMax];
    getcwd(path, PathMax);
    return Intern(path);
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

static bool UpdateStream(Entity entity, Stream& data) {
    if (IsStreamOpen(entity)) {
        StreamClose(entity);
    }

    data.StreamResolvedPath = ResolveVirtualPath(data.StreamPath);
    data.StreamProtocol = 0;

    char protocolIdentifier[32];
    auto colonLocation = strstr(data.StreamResolvedPath, "://");

    if(!colonLocation) {
        //Log(entity, LogSeverity_Error, "Malformed URI: %s (resolved from %s)", resolvedPath, data.StreamPath);
        return false;
    }

    u32 len = colonLocation - data.StreamResolvedPath;
    strncpy(protocolIdentifier, data.StreamResolvedPath, len);
    protocolIdentifier[len] = 0;
    auto protocolIdentifierInterned = Intern(protocolIdentifier);

    auto extension = GetFileExtension(data.StreamResolvedPath);

    StringRef mimeType = Intern("application/octet-stream");
    auto oldProtocol = data.StreamProtocol;

    // Find protocol
    StreamProtocol protocolData;
    for_entity_data(protocolEntity, ComponentOf_StreamProtocol(), &protocolData) {
        if(protocolIdentifierInterned == protocolData.StreamProtocolIdentifier) {
            data.StreamProtocol = protocolEntity;
            break;
        }
    }

    if(!IsEntityValid(data.StreamProtocol)) {
        Log(entity, LogSeverity_Error, "Unknown stream protocol: %s (%s)", protocolIdentifier, data.StreamResolvedPath);
        return false;
    }

    if(strlen(extension) > 0) {
        FileType fileTypeData;

        // Find Filetype (optional)
        for_entity_data(fileTypeEntity, ComponentOf_FileType(), &fileTypeData) {
            if(extension == fileTypeData.FileTypeExtension) {
                data.StreamFileType = fileTypeEntity;
                if(fileTypeData.FileTypeMimeType) {
                    mimeType = fileTypeData.FileTypeMimeType;
                    break;
                }
            }
        }

        if(!IsEntityValid(data.StreamFileType)) {
            Log(0, LogSeverity_Warning, "Unknown file type: %s", extension);
        }
    }

    // Find compressor (optional)
    StreamCompressor compressorData;
    for_entity_data(compressorEntity, ComponentOf_StreamCompressor(), &compressorData) {
        if(mimeType == compressorData.StreamCompressorMimeType) {
            data.StreamCompressor = compressorEntity;
        }
    }

    // Find serializer (optional)
    Serializer serializerData;
    for_entity_data(serializerEntity, ComponentOf_Serializer(), &serializerData) {
        if(mimeType == serializerData.SerializerMimeType) {
            AddComponent(entity, ComponentOf_PersistancePoint());
            break;
        }
    }

    return true;
}

static void OnStreamChanged(Entity entity, const Stream& oldData, const Stream& newData) {
    auto data = newData;

    if(oldData.StreamPath != data.StreamPath) {
        UpdateStream(entity, data);
        SetStream(entity, data);
    }
}

static void OnProtocolChanged(Entity protocol) {
    Stream streamData;
    for_entity_data(stream, ComponentOf_Stream(), &streamData) {
        if(streamData.StreamProtocol == protocol) {
            UpdateStream(stream, streamData);
            SetStream(stream, streamData);
        }
    }
}

static void OnFileTypeChanged(Entity fileType) {
    Stream streamData;
    for_entity_data(stream, ComponentOf_Stream(), &streamData) {
        if(streamData.StreamFileType == fileType) {
            UpdateStream(stream, streamData);
            SetStream(stream, streamData);
        }
    }
}

static void OnCompressorChanged(Entity compressor) {
    Stream streamData;
    for_entity_data(stream, ComponentOf_Stream(), &streamData) {
        if(streamData.StreamCompressor == compressor) {
            UpdateStream(stream, streamData);
            SetStream(stream, streamData);
        }
    }
}

static void OnVirtualPathChanged(Entity entity) {
    Stream streamData;
    for_entity_data(stream, ComponentOf_Stream(), &streamData) {
        UpdateStream(stream, streamData);
        SetStream(stream, streamData);
    }
}

static bool Compress(Entity entity, u64 offset, u64 size, const void *pixels) {
    return StreamWrite(entity, size, (char*)pixels + offset);
}

static bool Decompress(Entity entity, u64 offset, u64 size, void *pixels) {
    if(!StreamOpen(entity, StreamMode_Read)) return false;

    StreamSeek(entity, offset);
    auto success = StreamRead(entity, size, pixels) == size;
    StreamClose(entity);

    return success;
}

BeginUnit(Stream)
    BeginComponent(Stream)
        RegisterProperty(StringRef, StreamPath)
        RegisterPropertyReadOnly(StringRef, StreamResolvedPath)
        RegisterReferencePropertyReadOnly(StreamProtocol, StreamProtocol)
        RegisterReferencePropertyReadOnly(StreamCompressor, StreamCompressor)
        RegisterReferencePropertyReadOnly(FileType, StreamFileType)
        RegisterProperty(Date, StreamLastWrite)
    EndComponent()

    BeginComponent(StreamProtocol)
        RegisterProperty(StringRef, StreamProtocolIdentifier)
    EndComponent()

    BeginComponent(StreamCompressor)
        RegisterProperty(StringRef, StreamCompressorMimeType)
    EndComponent()

    BeginComponent(FileType)
        RegisterProperty(StringRef, FileTypeExtension)
        RegisterProperty(StringRef, FileTypeMimeType)
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

    RegisterStreamCompressor(Binary, "application/bin")
    RegisterFileType(Bin, "application/bin", ".bin")

    RegisterSystem(OnStreamChanged, ComponentOf_Stream())
    RegisterSystem(OnProtocolChanged, ComponentOf_StreamProtocol())
    RegisterSystem(OnFileTypeChanged, ComponentOf_FileType())
    RegisterSystem(OnCompressorChanged, ComponentOf_StreamCompressor())
    RegisterSystem(OnVirtualPathChanged, ComponentOf_VirtualPath())
EndUnit()



