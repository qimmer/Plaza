//
// Created by Kim on 21-08-2018.
//

#include <Core/Debug.h>
#include <Rendering/Mesh.h>
#include "BgfxMesh.h"
#include "BgfxResource.h"

#include <bgfx/bgfx.h>
#include <Foundation/Stream.h>
#include <Foundation/Invalidation.h>

struct BgfxVertexDeclaration {
    bgfx::VertexDecl decl;
};

struct BgfxVertexBuffer {
    u32 size;
    bool dynamic;
    u32 declHash;
};

struct BgfxIndexBuffer {
    u32 size;
    bool dynamic;
    bool isLong;
};

LocalFunction(OnBgfxVertexBufferRemoved, void, Entity entity) {
    auto data = GetBgfxVertexBufferData(entity);
    if(data) {
        bgfx::DynamicVertexBufferHandle dynHandle = { GetBgfxResourceHandle(entity) };
        bgfx::VertexBufferHandle staHandle = { dynHandle.idx };

        if(data->dynamic) {
            if(bgfx::isValid(dynHandle)) {
                bgfx::destroy(dynHandle);
            }
        } else {
            if(bgfx::isValid(staHandle)) {
                bgfx::destroy(staHandle);
            }
        }
    }
}

LocalFunction(OnBgfxIndexBufferRemoved, void, Entity entity) {
    auto data = GetBgfxIndexBufferData(entity);
    if(data) {
        bgfx::DynamicIndexBufferHandle dynHandle = { GetBgfxResourceHandle(entity) };
        bgfx::IndexBufferHandle staHandle = { dynHandle.idx };

        if(data->dynamic) {
            if(bgfx::isValid(dynHandle)) {
                bgfx::destroy(dynHandle);
            }
        } else {
            if(bgfx::isValid(staHandle)) {
                bgfx::destroy(staHandle);
            }
        }
    }
}

static void CalculateAABB(Entity vertexBuffer, const char *vertexData, u32 vertexDataSize) {
    auto decl = GetVertexBufferDeclaration(vertexBuffer);
    auto declData = GetBgfxVertexDeclarationData(decl);

    u8 numPositionElements = 0;
    bgfx::AttribType::Enum positionAttribType;
    bool normalized = false, asInt = false;
    declData->decl.decode(bgfx::Attrib::Position, numPositionElements, positionAttribType, normalized, asInt);

    auto stride = declData->decl.getStride();
    auto numVertices = vertexDataSize / stride;
    for(auto i = 0; i < numVertices; ++i) {

    }
}

static void ValidateVertexBuffer(Entity entity) {
    auto data = GetBgfxVertexBufferData(entity);
    auto decl = GetVertexBufferDeclaration(entity);

    auto declData = GetBgfxVertexDeclarationData(decl);

    if(!declData) return;

    if(declData->decl.m_hash == 0) return;

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


    // Create or update buffer
    if(GetVertexBufferDynamic(entity)) {
        auto handle = GetBgfxResourceHandle(entity);

        if(handle != bgfx::kInvalidHandle || !data->dynamic || data->declHash != declData->decl.m_hash) {
            OnBgfxVertexBufferRemoved(entity);
            SetBgfxResourceHandle(entity, bgfx::createDynamicVertexBuffer(bgfx::copy(buffer, size), declData->decl).idx);
        } else {
            bgfx::DynamicVertexBufferHandle specHandle = {handle};
            bgfx::update(specHandle, 0, bgfx::copy(buffer, size));
        }

        data->dynamic = true;
    } else {
        OnBgfxVertexBufferRemoved(entity);
        SetBgfxResourceHandle(entity, bgfx::createVertexBuffer(bgfx::copy(buffer, size), declData->decl).idx);

        data->dynamic = false;
    }

    data->size = size;
    data->declHash = declData->decl.m_hash;
    
    free(buffer);
}

static void ValidateIndexBuffer(Entity entity) {
    auto data = GetBgfxIndexBufferData(entity);
    auto isLong = GetIndexBufferLong(entity);

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


    // Create or update buffer
    if(GetIndexBufferDynamic(entity)) {
        auto handle = GetBgfxResourceHandle(entity);

        if(handle != bgfx::kInvalidHandle || !data->dynamic || data->isLong != isLong) {
            OnBgfxIndexBufferRemoved(entity);
            SetBgfxResourceHandle(entity, bgfx::createDynamicIndexBuffer(bgfx::copy(buffer, size), isLong ? BGFX_BUFFER_INDEX32 : BGFX_BUFFER_NONE).idx);
        } else {
            bgfx::DynamicIndexBufferHandle specHandle = {handle};
            bgfx::update(specHandle, 0, bgfx::copy(buffer, size));
        }

        data->dynamic = true;
    } else {
        OnBgfxIndexBufferRemoved(entity);
        SetBgfxResourceHandle(entity, bgfx::createIndexBuffer(bgfx::copy(buffer, size), isLong ? BGFX_BUFFER_INDEX32 : BGFX_BUFFER_NONE).idx);

        data->dynamic = false;
    }

    data->size = size;
    data->isLong = isLong;

    free(buffer);
}

static void ValidateVertexDeclaration(Entity entity) {
    auto data = GetBgfxVertexDeclarationData(entity);
    if(data) {
        data->decl.begin();
        for_children(attribute, VertexDeclarationAttributes, entity, {
            bgfx::AttribType::Enum elementType;
            u32 elementCount;
            if(GetVertexAttributeType(attribute) == TypeOf_float) {
                elementType = bgfx::AttribType::Float;
                elementCount = 1;
            } else if(GetVertexAttributeType(attribute) == TypeOf_v2f) {
                elementType = bgfx::AttribType::Float;
                elementCount = 2;
            } else if(GetVertexAttributeType(attribute) == TypeOf_v3f) {
                elementType = bgfx::AttribType::Float;
                elementCount = 3;
            } else if(GetVertexAttributeType(attribute) == TypeOf_v4f) {
                elementType = bgfx::AttribType::Float;
                elementCount = 4;
            } else if(GetVertexAttributeType(attribute) == TypeOf_rgba8) {
                elementType = bgfx::AttribType::Uint8;
                elementCount = 4;
            } else if(GetVertexAttributeType(attribute) == TypeOf_s16) {
                elementType = bgfx::AttribType::Int16;
                elementCount = 1;
            } else {
                Log(entity, LogSeverity_Error, "Unsupported attribute type: %s", GetTypeName(GetVertexAttributeType(attribute)));
                continue;
            }

            auto elementUsage = (bgfx::Attrib::Enum)GetVertexAttributeUsage(attribute);
            data->decl.add(
                    elementUsage,
                    elementCount,
                    elementType,
                    GetVertexAttributeNormalize(attribute),
                    GetVertexAttributeAsInt(attribute));
        });

        data->decl.end();
    }

    for_entity(vertexBuffer, vertexBufferData, VertexBuffer, {
        if(GetVertexBufferDeclaration(vertexBuffer) == entity) {
            Invalidate(vertexBuffer);
        }
    });
}

LocalFunction(OnVertexDeclarationValidation, void, Entity component) {
    for_entity(entity, data, VertexDeclaration, {
        if(!IsDirty(entity)) continue;
        ValidateVertexDeclaration(entity);
    });
}

LocalFunction(OnVertexBufferValidation, void, Entity component) {
    for_entity(entity, data, VertexBuffer, {
        if(!IsDirty(entity)) continue;
        ValidateVertexBuffer(entity);
    });
}

LocalFunction(OnIndexBufferValidation, void, Entity component) {
    for_entity(entity, data, IndexBuffer, {
        if(!IsDirty(entity)) continue;
        ValidateIndexBuffer(entity);
    });
}

BeginUnit(BgfxMesh)
    BeginComponent(BgfxVertexDeclaration)
        RegisterBase(BgfxResource)
    EndComponent()
    BeginComponent(BgfxVertexBuffer)
        RegisterBase(BgfxResource)
    EndComponent()
    BeginComponent(BgfxIndexBuffer)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentRemoved(), OnBgfxIndexBufferRemoved, ComponentOf_BgfxIndexBuffer())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnBgfxVertexBufferRemoved, ComponentOf_BgfxVertexBuffer())
    RegisterSubscription(EventOf_Validate(), OnVertexDeclarationValidation, ComponentOf_VertexDeclaration())
    RegisterSubscription(EventOf_Validate(), OnVertexBufferValidation, ComponentOf_VertexBuffer())
    RegisterSubscription(EventOf_Validate(), OnIndexBufferValidation, ComponentOf_IndexBuffer())
EndUnit()