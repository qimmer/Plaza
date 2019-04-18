//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_STREAM_H
#define PLAZA_STREAM_H

#include <Core/NativeUtils.h>
#include <Foundation/NativeUtils.h>

typedef bool(*StreamSeekHandlerType)(Entity entity, s32 offset);
typedef s32(*StreamTellHandlerType)(Entity entity);
typedef bool(*StreamOpenHandlerType)(Entity entity, int mode);
typedef u64(*StreamReadHandlerType)(Entity entity, u64 size, void* data);
typedef u64(*StreamWriteHandlerType)(Entity entity, u64 size, const void* data);
typedef void(*StreamReadAsyncHandler)(Entity entity, u64 numBytes, const void *data);
typedef void(*StreamWriteAsyncHandler)(Entity entity, u64 numBytes);
typedef StringRef(*StreamMimeTypeHandlerType)(Entity entity);
typedef bool(*StreamBoolHandlerType)(Entity entity);

typedef bool(*CompressHandlerType)(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, const void *uncompressedData);
typedef bool(*DecompressHandlerType)(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData);

struct Serializer {
    StringRef SerializerMimeType;
    bool(*SerializeHandler)(Entity entity);
    bool(*DeserializeHandler)(Entity entity);
};

struct Stream {
    StringRef StreamPath;
    StringRef StreamResolvedPath;
    Date StreamLastWrite;

    Entity StreamProtocol, StreamCompressor, StreamFileType;

    int StreamMode;

    bool InvalidationPending;
};

struct FileType {
    StringRef FileTypeExtension;
    StringRef FileTypeMimeType;
};

struct StreamExtensionModule {
    ChildArray ModuleStreamProtocols, ModuleStreamCompressors, ModuleFileTypes, ModuleSerializers;
};

struct StreamProtocol {
    StringRef StreamProtocolIdentifier;
    StreamSeekHandlerType StreamSeekHandler;
    StreamTellHandlerType StreamTellHandler;
    StreamReadHandlerType StreamReadHandler;
    StreamWriteHandlerType StreamWriteHandler;
    StreamBoolHandlerType StreamIsOpenHandler;
    StreamOpenHandlerType StreamOpenHandler;
    StreamBoolHandlerType StreamCloseHandler;
    StreamBoolHandlerType StreamDeleteHandler;
    StreamMimeTypeHandlerType StreamGetMimeTypeHandler;
};

struct StreamCompressor {
    StringRef StreamCompressorMimeType;
    CompressHandlerType CompressHandler;
    DecompressHandlerType DecompressHandler;
};

Unit(Stream)
    Component(Stream)
        Property(StringRef, StreamPath)
        Property(StringRef, StreamResolvedPath)
        Property(Date, StreamLastWrite)
        ReferenceProperty(Entity, StreamProtocol)
        ReferenceProperty(Entity, StreamCompressor)
        ReferenceProperty(Entity, StreamFileType)

    Component(StreamProtocol)
        Property(StringRef, StreamProtocolIdentifier)

    Component(StreamCompressor)
        Property(StringRef, StreamCompressorMimeType)

    Component(FileType)
        Property(StringRef, FileTypeExtension)
        Property(StringRef, FileTypeMimeType)

    Component(Serializer)
        Property(StringRef, SerializerMimeType)

    Component(StreamExtensionModule)
        ArrayProperty(StreamProtocol, ModuleStreamProtocols)
        ArrayProperty(StreamCompressor, ModuleStreamCompressors)
        ArrayProperty(FileType, ModuleFileTypes)
        ArrayProperty(Serializer, ModuleSerializers)

    FileType(Bin)
    StreamCompressor(Binary)

#define StreamMode_Closed 0
#define StreamMode_Read 1
#define StreamMode_Write 2

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
StringRef GetParentFolder(StringRef absolutePath);
StringRef GetFileName(StringRef absolutePath);
StringRef GetFileExtension(StringRef absolutePath);
void CleanupPath(char* messyPath);
StringRef GetCurrentWorkingDirectory();

bool SetStreamData(Entity stream, u32 offset, u32 numBytes, const char *data);

void ResolveStreamPath(Entity stream);

#define StreamSeek_End -1

#endif //PLAZA_STREAM_H
