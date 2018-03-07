//
// Created by Kim Johannsen on 16/01/2018.
//

#include <bgfx/bgfx.h>
#include <Rendering/IndexBuffer.h>
#include "BgfxIndexBuffer.h"
#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include <Core/Vector.h>
#include <Foundation/Invalidation.h>


    struct BgfxIndexBuffer {
        BgfxIndexBuffer() :
                staticHandle(BGFX_INVALID_HANDLE),
                dynamicHandle(BGFX_INVALID_HANDLE),
                size(0) {}

        bgfx::IndexBufferHandle staticHandle;
        bgfx::DynamicIndexBufferHandle dynamicHandle;
        u32 size;
    };

    DefineComponent(BgfxIndexBuffer)
    EndComponent()

    DefineService(BgfxIndexBuffer)
    EndService()

    void UpdateBgfxIndexBuffer(Entity entity) {
        if(!HasBgfxIndexBuffer(entity))
        {
            return;
        }

        auto data = GetBgfxIndexBuffer(entity);
        if(!StreamOpen(entity, StreamMode_Read)) return;
        StreamSeek(entity, StreamSeek_End);
        auto size = StreamTell(entity);

        if(size == 0) {
            StreamClose(entity);
            return;
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
    }

    void OnIndexBufferRemoved(Entity entity) {
        auto data = GetBgfxIndexBuffer(entity);

        if(bgfx::isValid(data->staticHandle)) {
            bgfx::destroy(data->staticHandle);
            data->staticHandle = BGFX_INVALID_HANDLE;
        }
    }

    static bool ServiceStart() {
        SubscribeBgfxIndexBufferRemoved(OnIndexBufferRemoved);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeBgfxIndexBufferRemoved(OnIndexBufferRemoved);
        return true;
    }

    u16 GetBgfxIndexBufferHandle(Entity entity) {
        if(GetIndexBufferDynamic(entity)) {
            return GetBgfxIndexBuffer(entity)->dynamicHandle.idx;
        } else {
            return GetBgfxIndexBuffer(entity)->staticHandle.idx;
        }
    }

