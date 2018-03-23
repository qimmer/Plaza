//
// Created by Kim Johannsen on 16/01/2018.
//

#include <bgfx/bgfx.h>
#include <Rendering/VertexBuffer.h>
#include "BgfxVertexBuffer.h"
#include "BgfxVertexDeclaration.h"
#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include <Core/Vector.h>
#include <Core/Hierarchy.h>
#include <Rendering/VertexAttribute.h>


struct BgfxVertexBuffer {
    BgfxVertexBuffer() :
            staticHandle(BGFX_INVALID_HANDLE),
            dynamicHandle(BGFX_INVALID_HANDLE), invalidated(true), size(0) {}

    bgfx::VertexBufferHandle staticHandle;
    bgfx::DynamicVertexBufferHandle dynamicHandle;
    u32 size;
    bool invalidated;
};

DefineComponent(BgfxVertexBuffer)
EndComponent()

DefineService(BgfxVertexBuffer)
EndService()

static void OnChanged(Entity entity) {
    if(HasBgfxVertexBuffer(entity)) {
        GetBgfxVertexBuffer(entity)->invalidated = true;
    }
}

void OnVertexBufferRemoved(Entity entity) {
    auto data = GetBgfxVertexBuffer(entity);

    if(bgfx::isValid(data->staticHandle)) {
        bgfx::destroy(data->staticHandle);
        data->staticHandle = BGFX_INVALID_HANDLE;
    }
}

static bool ServiceStart() {
    SubscribeBgfxVertexBufferRemoved(OnVertexBufferRemoved);
    SubscribeVertexBufferChanged(OnChanged);
    SubscribeStreamChanged(OnChanged);
    SubscribeStreamContentChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeBgfxVertexBufferRemoved(OnVertexBufferRemoved);
    UnsubscribeVertexBufferChanged(OnChanged);
    UnsubscribeStreamChanged(OnChanged);
    UnsubscribeStreamContentChanged(OnChanged);
    return true;
}

u16 GetBgfxVertexBufferHandle(Entity entity) {
    auto data = GetBgfxVertexBuffer(entity);

    if(data->invalidated) {
        auto declData = (bgfx::VertexDecl*)GetBgfxVertexDeclarationHandle(GetVertexBufferDeclaration(entity));

        if(declData->m_hash == 0) return bgfx::kInvalidHandle;

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

        if(GetVertexBufferDynamic(entity)) {
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
        if(GetVertexBufferDynamic(entity)) {
            if(!bgfx::isValid(data->dynamicHandle)) {
                data->dynamicHandle = bgfx::createDynamicVertexBuffer(bgfx::copy(buffer, size), *declData);
            } else {
                bgfx::updateDynamicVertexBuffer(data->dynamicHandle, 0, bgfx::copy(buffer, size));
            }
        } else {
            data->staticHandle = bgfx::createVertexBuffer(bgfx::copy(buffer, size), *declData);
        }

        data->size = size;
        free(buffer);

        data->invalidated = false;
    }

    if(GetVertexBufferDynamic(entity)) {
        return GetBgfxVertexBuffer(entity)->dynamicHandle.idx;
    } else {
        return GetBgfxVertexBuffer(entity)->staticHandle.idx;
    }
}

