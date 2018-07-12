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
#include <Core/Node.h>
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

BeginUnit(BgfxVertexBuffer)
    BeginComponent(BgfxVertexBuffer)
EndComponent()

LocalFunction(OnChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxVertexBuffer())) {
        GetBgfxVertexBufferData(entity)->invalidated = true;
    }
}

void OnVertexBufferRemoved(Entity entity) {
    auto data = GetBgfxVertexBufferData(entity);

    if(bgfx::isValid(data->staticHandle)) {
        bgfx::destroy(data->staticHandle);
        data->staticHandle = BGFX_INVALID_HANDLE;
    }
}

u16 GetBgfxVertexBufferHandle(Entity entity) {
    auto data = GetBgfxVertexBufferData(entity);

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
        return GetBgfxVertexBufferData(entity)->dynamicHandle.idx;
    } else {
        return GetBgfxVertexBufferData(entity)->staticHandle.idx;
    }
}

DefineService(BgfxVertexBuffer)
        RegisterSubscription(BgfxVertexBufferRemoved, OnVertexBufferRemoved, 0)
        RegisterSubscription(VertexBufferChanged, OnChanged, 0)
        RegisterSubscription(StreamChanged, OnChanged, 0)
        RegisterSubscription(StreamContentChanged, OnChanged, 0)
EndService()
