//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_STREAM_H
#define PLAZA_STREAM_H

#include <Core/NativeUtils.h>

struct Serializer {
    char SerializerMimeType[64];
    bool(*SerializeHandler)(Entity entity);
    bool(*DeserializeHandler)(Entity entity);
};

Unit(Stream)
    Component(Stream)
        Property(StringRef, StreamPath)
        Property(Entity, StreamProtocol, PropertyFlag_ReadOnly, PropertyFlag_Transient)
        Property(Entity, StreamCompressor, PropertyFlag_ReadOnly, PropertyFlag_Transient)
        Property(Entity, StreamFileType, PropertyFlag_ReadOnly, PropertyFlag_Transient)

    Component(StreamProtocol)
        Property(StringRef, StreamProtocolIdentifier)
        Property(Entity, StreamProtocolComponent)

    Component(StreamCompressor)
        Property(StringRef, StreamCompressorMimeType)

    Component(FileType)
        Property(StringRef, FileTypeExtension)
        Property(StringRef, FileTypeMimeType)
        Property(Entity, FileTypeComponent)

    Component(Serializer)
        Property(StringRef, SerializerMimeType)

    Component(StreamExtensionModule)
        ArrayProperty(StreamProtocol, ModuleStreamProtocols)
        ArrayProperty(StreamCompressor, ModuleStreamCompressors)
        ArrayProperty(FileType, ModuleFileTypes)
        ArrayProperty(Serializer, ModuleSerializers)


Event(StreamContentChanged)

typedef bool(*StreamSeekHandler)(Entity entity, s32 offset);
typedef s32(*StreamTellHandler)(Entity entity);
typedef bool(*StreamOpenHandler)(Entity entity, int mode);
typedef u64(*StreamReadHandler)(Entity entity, u64 size, void* data);
typedef u64(*StreamWriteHandler)(Entity entity, u64 size, const void* data);
typedef void(*StreamReadAsyncHandler)(Entity entity, u64 numBytes, const void *data);
typedef void(*StreamWriteAsyncHandler)(Entity entity, u64 numBytes);
typedef StringRef(*StreamMimeTypeHandler)(Entity entity);
typedef bool(*StreamBoolHandler)(Entity entity);

typedef bool(*CompressHandler)(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, const void *uncompressedData);
typedef bool(*DecompressHandler)(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData);

struct StreamProtocol {
    char StreamProtocolIdentifier[16];
    Entity StreamProtocolComponent;
    StreamSeekHandler StreamSeekHandler;
    StreamTellHandler StreamTellHandler;
    StreamReadHandler StreamReadHandler;
    StreamWriteHandler StreamWriteHandler;
    StreamBoolHandler StreamIsOpenHandler;
    StreamOpenHandler StreamOpenHandler;
    StreamBoolHandler StreamCloseHandler;
    StreamBoolHandler StreamDeleteHandler;
    StreamMimeTypeHandler StreamGetMimeTypeHandler;
};

struct StreamCompressor {
    char StreamCompressorMimeType[64];
    CompressHandler CompressHandler;
    DecompressHandler DecompressHandler;
};

#define StreamMode_Closed 0
#define StreamMode_Read 1
#define StreamMode_Write 2

StringRef GetStreamResolvedPath(Entity entity);

bool StreamCopy(Entity source, Entity destination);

bool StreamDelete(Entity entity);
bool StreamSeek(Entity entity, s32 offset);
s32 StreamTell(Entity entity);
u64 StreamRead(Entity entity, u64 size, void *data);
u64 StreamWrite(Entity entity, u64 size, const void *data);

u64 StreamDecompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData);
u64 StreamCompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, const void *uncompressedData);

bool IsStreamOpen(Entity entity);
int GetStreamMode(Entity entity);
bool StreamOpen(Entity entity, int mode);
bool StreamClose(Entity entity);

void StreamReadAsync(Entity entity, u64 size, StreamReadAsyncHandler readFinishedHandler);
void StreamWriteAsync(Entity entity, u64 size, const void *data, StreamWriteAsyncHandler writeFinishedHandler);

char GetPathSeparator();
void GetParentFolder(StringRef absolutePath, char *parentFolder, size_t maxBuf);
StringRef GetFileName(StringRef absolutePath);
StringRef GetFileExtension(StringRef absolutePath);
void CleanupPath(char* messyPath);
StringRef GetCurrentWorkingDirectory();

bool SetStreamData(Entity stream, u32 offset, u32 numBytes, const char *data);

#define StreamSeek_End -1

#endif //PLAZA_STREAM_H
