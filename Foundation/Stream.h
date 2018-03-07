//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_STREAM_H
#define PLAZA_STREAM_H

#include <Core/Entity.h>

typedef bool(*StreamSeekHandler)(Entity entity, s32 offset);
typedef s32(*StreamTellHandler)(Entity entity);
typedef bool(*StreamOpenHandler)(Entity entity, int mode);
typedef u64(*StreamReadHandler)(Entity entity, u64 size, void* data);
typedef u64(*StreamWriteHandler)(Entity entity, u64 size, const void* data);
typedef void(*StreamAsyncHandler)(Entity entity, u64 numBytes);
typedef StringRef(*StreamMimeTypeHandler)(Entity entity);

typedef struct StreamProtocol {
    EntityBoolHandler StreamProtocolSet;
    EntityBoolHandler StreamProtocolUnset;
    StreamSeekHandler StreamSeekHandler;
    StreamTellHandler StreamTellHandler;
    StreamReadHandler StreamReadHandler;
    StreamWriteHandler StreamWriteHandler;
    EntityBoolHandler StreamIsOpenHandler;
    StreamOpenHandler StreamOpenHandler;
    EntityHandler StreamCloseHandler;
    StreamMimeTypeHandler StreamGetMimeTypeHandler;
} StreamProtocol;

typedef bool(*CompressHandler)(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, const void *uncompressedData);
typedef bool(*DecompressHandler)(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData);

typedef struct StreamCompressor {
    CompressHandler CompressHandler;
    DecompressHandler DecompressHandler;
} StreamCompressor;

#define StreamMode_Closed 0
#define StreamMode_Read 1
#define StreamMode_Write 2

DeclareComponent(Stream)
DeclareService(Stream)

DeclareComponentProperty(Stream, StringRef, StreamPath)

StringRef GetStreamResolvedPath(Entity entity);

bool StreamSeek(Entity entity, s32 offset);
s32 StreamTell(Entity entity);
u64 StreamRead(Entity entity, u64 size, void *data);
u64 StreamWrite(Entity entity, u64 size, const void *data);

u64 StreamDecompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, void *uncompressedData);
u64 StreamCompress(Entity entity, u64 uncompressedOffset, u64 uncompressedSize, const void *uncompressedData);

bool IsStreamOpen(Entity entity);
int GetStreamMode(Entity entity);
StringRef GetStreamMimeType(Entity entity);
bool StreamOpen(Entity entity, int mode);
void StreamClose(Entity entity);

void StreamReadAsync(Entity entity, u64 size, void *data, StreamAsyncHandler readFinishedHandler);
void StreamWriteAsync(Entity entity, u64 size, const void *data, StreamAsyncHandler writeFinishedHandler);

void AddStreamProtocol(StringRef protocolIdentifier, struct StreamProtocol *protocol);
void RemoveStreamProtocol(StringRef protocolIdentifier);

void AddStreamCompressor(StringRef mimeType, struct StreamCompressor *compressor);
void RemoveStreamCompressor(StringRef mimeType);

void AddFileType(StringRef fileExtension, StringRef mimeType);
void RemoveFileType(StringRef fileExtension);

DeclareEvent(StreamContentChanged, EntityHandler)

#define StreamSeek_End UINT32_MAX

#endif //PLAZA_STREAM_H
