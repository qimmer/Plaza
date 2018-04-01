//
// Created by Kim Johannsen on 16/01/2018.
//

#include <bgfx/bgfx.h>
#include <Rendering/IndexBuffer.h>
#include "BgfxIndexBuffer.h"
#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include <Core/Vector.h>

struct BgfxIndexBuffer {
    BgfxIndexBuffer() :
            staticHandle(BGFX_INVALID_HANDLE),
            dynamicHandle(BGFX_INVALID_HANDLE),
            size(0), invalidated(true) {}

    bgfx::IndexBufferHandle staticHandle;
    bgfx::DynamicIndexBufferHandle dynamicHandle;
    u32 size;
    bool invalidated;
};

DefineComponent(BgfxIndexBuffer)
EndComponent()

void OnIndexBufferRemoved(Entity entity) {
    auto data = GetBgfxIndexBuffer(entity);

    if(bgfx::isValid(data->staticHandle)) {
        bgfx::destroy(data->staticHandle);
        data->staticHandle = BGFX_INVALID_HANDLE;
    }
}

static void OnChanged(Entity entity) {
    if(HasBgfxIndexBuffer(entity)) {
        GetBgfxIndexBuffer(entity)->invalidated = true;
    }
}

DefineService(BgfxIndexBuffer)
        Subscribe(BgfxIndexBufferRemoved, OnIndexBufferRemoved)
        Subscribe(IndexBufferChanged, OnChanged)
        Subscribe(StreamChanged, OnChanged)
        Subscribe(StreamContentChanged, OnChanged)
EndService()

u16 GetBgfxIndexBufferHandle(Entity entity) {
    auto data = GetBgfxIndexBuffer(entity);
    if(data->invalidated) {
        if(!StreamOpen(entity, StreamMode_Read)) return bgfx::kInvalidHandle;
        StreamSeek(entity, StreamSeek_End);
        auto size = StreamTell(entity);

        if(size == 0) {
            StreamClose(entity);
            return bgfx::kInvalidHandle;
        }

        auto buffer = malloc(size);
        StreamSeek(entity, 0);
        StreamRead(entity, size, buffer);
        StreamClose(entity);

        // Eventually free old buffers
        if(bgfx::isValid(data->staticHandle)) {
            bgfx::destroy(data->staticHandle);
            data->staticHandle = BGFX_INVALID_HANDLE;
        }

        if(GetIndexBufferDynamic(entity)) {
            if(data->size != size) {
                if(bgfx::isValid(data->dynamicHandle)) {
                    bgfx::destroy(data->dynamicHandle);
                    data->dynamicHandle = BGFX_INVALID_HANDLE;
                }
            }
        } else {
            if(bgfx::isValid(data->dynamicHandle)) {
                bgfx::destroy(data->dynamicHandle);
                data->dynamicHandle = BGFX_INVALID_HANDLE;
            }
        }

        // Create or update buffer
        if(GetIndexBufferDynamic(entity)) {
            if(!bgfx::isValid(data->dynamicHandle)) {
                data->dynamicHandle = bgfx::createDynamicIndexBuffer(bgfx::copy(buffer, size), GetIndexBufferLong(entity) ? BGFX_BUFFER_INDEX32 : 0);
            } else {
                bgfx::updateDynamicIndexBuffer(data->dynamicHandle, 0, bgfx::copy(buffer, size));
            }
        } else {
            data->staticHandle = bgfx::createIndexBuffer(bgfx::copy(buffer, size), GetIndexBufferLong(entity) ? BGFX_BUFFER_INDEX32 : 0);
        }

        data->size = size;
        free(buffer);

        data->invalidated = false;
    }

    if(data->dynamicHandle.idx != bgfx::kInvalidHandle) {
        return data->dynamicHandle.idx;
    } else {
        return data->staticHandle.idx;
    }
}

