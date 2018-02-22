//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_STREAM_H
#define PLAZA_STREAM_H

#include <Core/Entity.h>


    typedef bool(*StreamSeekHandler)(Entity entity, s32 offset);
    typedef s32(*StreamTellHandler)(Entity entity);
    typedef u64(*StreamReadHandler)(Entity entity, u64 size, void* data);
    typedef u64(*StreamWriteHandler)(Entity entity, u64 size, const void* data);
    typedef void(*StreamAsyncHandler)(Entity entity, u64 numBytes);

    DeclareComponent(Stream)

    DeclareComponentProperty(Stream, StreamSeekHandler, StreamSeekHandler)
    DeclareComponentProperty(Stream, StreamTellHandler, StreamTellHandler)
    DeclareComponentProperty(Stream, StreamReadHandler, StreamReadHandler)
    DeclareComponentProperty(Stream, StreamWriteHandler, StreamWriteHandler)
    DeclareComponentProperty(Stream, EntityBoolHandler, StreamIsOpenHandler)
    DeclareComponentProperty(Stream, EntityBoolHandler, StreamOpenHandler)
    DeclareComponentProperty(Stream, EntityHandler, StreamCloseHandler)
    DeclareComponentProperty(Stream, StringRef, StreamMimeType)

    bool StreamSeek(Entity entity, s32 offset);
    s32 StreamTell(Entity entity);
    u64 StreamRead(Entity entity, u64 size, void *data);
    u64 StreamWrite(Entity entity, u64 size, const void *data);
    bool IsStreamOpen(Entity entity);
    bool StreamOpen(Entity entity);
    void StreamClose(Entity entity);

    void StreamReadAsync(Entity entity, u64 size, void *data, StreamAsyncHandler readFinishedHandler);
    void StreamWriteAsync(Entity entity, u64 size, const void *data, StreamAsyncHandler writeFinishedHandler);

#define STREAM_SEEK_END UINT32_MAX

#endif //PLAZA_STREAM_H
