//
// Created by Kim Johannsen on 16/01/2018.
//

#include <bgfx/bgfx.h>
#include <Rendering/VertexBuffer.h>
#include "BgfxVertexBuffer.h"
#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include <Core/Vector.h>
#include <Foundation/Invalidation.h>
#include <Core/Hierarchy.h>
#include <Rendering/VertexAttribute.h>


    struct BgfxVertexBuffer {
        BgfxVertexBuffer() :
                staticHandle(BGFX_INVALID_HANDLE),
                dynamicHandle(BGFX_INVALID_HANDLE) {}

        bgfx::VertexBufferHandle staticHandle;
        bgfx::DynamicVertexBufferHandle dynamicHandle;
        u32 size;
    };

    struct BgfxVertexDeclaration {
        bgfx::VertexDecl decl;
    };

    DefineComponent(BgfxVertexBuffer)
    EndComponent()

    DefineComponent(BgfxVertexDeclaration)
    EndComponent()

    DefineService(BgfxVertexBuffer)
    EndService()

    void UpdateBgfxVertexBuffer(Entity entity) {
        if(!HasBgfxVertexBuffer(entity)) {
            return;
        }
        auto vertexDeclaration = GetVertexBufferDeclaration(entity);
        if(!IsEntityValid(vertexDeclaration) || !HasBgfxVertexDeclaration(vertexDeclaration)) {
            return;
        }

        auto data = GetBgfxVertexBuffer(entity);
        auto stream = GetVertexBufferStream(entity);
        Assert(StreamOpen(stream));
        StreamSeek(stream, STREAM_SEEK_END);
        auto size = StreamTell(stream);

        if(size == 0) {
            StreamClose(stream);
            return;
        }

        auto buffer = alloca(size);
        StreamSeek(stream, 0);
        StreamRead(stream, size, buffer);
        StreamClose(stream);

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
                data->dynamicHandle = bgfx::createDynamicVertexBuffer(bgfx::copy(buffer, size), GetBgfxVertexDeclaration(vertexDeclaration)->decl);
            } else {
                bgfx::updateDynamicVertexBuffer(data->dynamicHandle, 0, bgfx::copy(buffer, size));
            }
        } else {
            data->staticHandle = bgfx::createVertexBuffer(bgfx::copy(buffer, size), GetBgfxVertexDeclaration(vertexDeclaration)->decl);
        }

        data->size = size;
    }

    void UpdateBgfxVertexDeclaration(Entity entity) {
        if(!HasBgfxVertexDeclaration(entity)) return;

        auto data = GetBgfxVertexDeclaration(entity);

        data->decl.begin();
        for(auto attribute = GetFirstChild(entity); IsEntityValid(attribute); attribute = GetSibling(attribute)) {
            if(!HasVertexAttribute(attribute)) continue;

            bgfx::AttribType::Enum elementType;
            u32 elementCount;
            if(GetVertexAttributeType(attribute) == TypeOf_float()) {
                elementType = bgfx::AttribType::Float;
                elementCount = 1;
            } else if(GetVertexAttributeType(attribute) == TypeOf_v2f()) {
                elementType = bgfx::AttribType::Float;
                elementCount = 2;
            } else if(GetVertexAttributeType(attribute) == TypeOf_v3f()) {
                elementType = bgfx::AttribType::Float;
                elementCount = 3;
            } else if(GetVertexAttributeType(attribute) == TypeOf_v4f()) {
                elementType = bgfx::AttribType::Float;
                elementCount = 4;
            } else if(GetVertexAttributeType(attribute) == TypeOf_rgba8()) {
                elementType = bgfx::AttribType::Uint8;
                elementCount = 4;
            } else if(GetVertexAttributeType(attribute) == TypeOf_s16()) {
                elementType = bgfx::AttribType::Int16;
                elementCount = 1;
            } else {
                Log(LogChannel_Core, LogSeverity_Error, "Unsupported attribute type: %s", GetTypeName(GetVertexAttributeType(attribute)));
                continue;
            }

            auto elementUsage = (bgfx::Attrib::Enum)GetVertexAttributeUsage(attribute);
            data->decl.add(
                    elementUsage,
                    elementCount,
                    elementType,
                    GetVertexAttributeNormalize(attribute),
                    GetVertexAttributeAsInt(attribute));
        }
        data->decl.end();
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
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeBgfxVertexBufferRemoved(OnVertexBufferRemoved);
        return true;
    }

    u16 GetBgfxVertexBufferHandle(Entity entity) {
        if(GetVertexBufferDynamic(entity)) {
            return GetBgfxVertexBuffer(entity)->dynamicHandle.idx;
        } else {
            return GetBgfxVertexBuffer(entity)->staticHandle.idx;
        }
    }

