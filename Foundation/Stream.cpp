//
// Created by Kim Johannsen on 13/01/2018.
//

#include <Core/String.h>
#include "Stream.h"
#include "Invalidation.h"

struct Stream {
    Stream() {
        memset(this, 0, sizeof(Stream));
    }

    StreamSeekHandler StreamSeekHandler;
    StreamReadHandler StreamReadHandler;
    StreamWriteHandler StreamWriteHandler;
    StreamTellHandler StreamTellHandler;
    EntityBoolHandler StreamOpenHandler, StreamIsOpenHandler;
    EntityHandler StreamCloseHandler;
    bool InvalidationPending;
    String StreamMimeType;
};

DefineComponent(Stream)
    Abstract()
    Dependency(Invalidation)
EndComponent()

DefineComponentProperty(Stream, StreamSeekHandler, StreamSeekHandler)
DefineComponentProperty(Stream, StreamTellHandler, StreamTellHandler)
DefineComponentProperty(Stream, StreamReadHandler, StreamReadHandler)
DefineComponentProperty(Stream, StreamWriteHandler, StreamWriteHandler)
DefineComponentProperty(Stream, EntityBoolHandler, StreamIsOpenHandler)
DefineComponentProperty(Stream, EntityBoolHandler, StreamOpenHandler)
DefineComponentProperty(Stream, EntityHandler, StreamCloseHandler)
DefineComponentProperty(Stream, StringRef, StreamMimeType)

bool StreamSeek(Entity entity, s32 offset){
    if(!GetStream(entity)->StreamSeekHandler) {
        return false;
    }

    return GetStream(entity)->StreamSeekHandler(entity, offset);
}

s32 StreamTell(Entity entity){
    if(!GetStream(entity)->StreamTellHandler) {
        return 0;
    }

    return GetStream(entity)->StreamTellHandler(entity);
}

u64 StreamRead(Entity entity, u64 size, void *data){
    if(!GetStream(entity)->StreamReadHandler) {
        return 0;
    }

    return GetStream(entity)->StreamReadHandler(entity, size, data);
}

u64 StreamWrite(Entity entity, u64 size, const void *data){
    if(!GetStream(entity)->StreamWriteHandler) {
        return 0;
    }

    GetStream(entity)->InvalidationPending = true;

    return GetStream(entity)->StreamWriteHandler(entity, size, data);
}

bool IsStreamOpen(Entity entity) {
    if(!GetStream(entity)->StreamIsOpenHandler) {
        return false;
    }

    return GetStream(entity)->StreamIsOpenHandler(entity);
}

bool StreamOpen(Entity entity) {
    if(!GetStream(entity)->StreamOpenHandler) {
        return false;
    }

    return GetStream(entity)->StreamOpenHandler(entity);
}

void StreamClose(Entity entity) {
    if(GetStream(entity)->StreamCloseHandler) {
        GetStream(entity)->StreamCloseHandler(entity);

        if(GetStream(entity)->InvalidationPending) {
            GetStream(entity)->InvalidationPending = false;
            SetInvalidated(entity, true);
        }
    }
}

void StreamReadAsync(Entity entity, u64 size, void *data, StreamAsyncHandler readFinishedHandler){

}

void StreamWriteAsync(Entity entity, u64 size, const void *data, StreamAsyncHandler writeFinishedHandler){

}

