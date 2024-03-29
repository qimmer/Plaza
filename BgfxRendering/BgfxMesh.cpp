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
    bgfx::DynamicVertexBufferHandle dynHandle = { GetBgfxResourceHandle(entity) };
    bgfx::VertexBufferHandle staHandle = { dynHandle.idx };

    auto data = GetBgfxVertexBufferData(entity);

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

LocalFunction(OnBgfxIndexBufferRemoved, void, Entity entity) {
    bgfx::DynamicIndexBufferHandle dynHandle = { GetBgfxResourceHandle(entity) };
    bgfx::IndexBufferHandle staHandle = { dynHandle.idx };

    auto data = GetBgfxIndexBufferData(entity);

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

static void ValidateVertexBuffer(Entity entity) {
    auto data = GetBgfxVertexBufferData(entity);
    auto& declData = GetBgfxVertexDeclarationData(GetVertexBufferDeclaration(entity))->decl;

    if(declData.m_hash == 0) return;

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

        if(handle != bgfx::kInvalidHandle || !data->dynamic || data->declHash != declData.m_hash) {
            OnBgfxVertexBufferRemoved(entity);
            SetBgfxResourceHandle(entity, bgfx::createDynamicVertexBuffer(bgfx::copy(buffer, size), declData).idx);
        } else {
            bgfx::DynamicVertexBufferHandle specHandle = {handle};
            bgfx::update(specHandle, 0, bgfx::copy(buffer, size));
        }

        data->dynamic = true;
    } else {
        OnBgfxVertexBufferRemoved(entity);
        SetBgfxResourceHandle(entity, bgfx::createVertexBuffer(bgfx::copy(buffer, size), declData).idx);

        data->dynamic = false;
    }

    data->size = size;
    data->declHash = declData.m_hash;
    
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
    data->decl.begin();
    for_children(attribute, VertexDeclarationAttributes, entity) {
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
    }
    data->decl.end();

    for_entity(vertexBuffer, vertexBufferData, VertexBuffer) {
        if(GetVertexBufferDeclaration(vertexBuffer) == entity) {
            Invalidate(vertexBuffer);
        }
    }
}

LocalFunction(OnValidation, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxVertexDeclaration())) {
        ValidateVertexDeclaration(entity);
    }

    if(HasComponent(entity, ComponentOf_BgfxVertexBuffer())) {
        ValidateVertexBuffer(entity);
    }

    if(HasComponent(entity, ComponentOf_BgfxIndexBuffer())) {
        ValidateIndexBuffer(entity);
    }
}

BeginUnit(BgfxMesh)
    BeginComponent(BgfxVertexDeclaration)
    EndComponent()
    BeginComponent(BgfxVertexBuffer)
    EndComponent()
    BeginComponent(BgfxIndexBuffer)
    EndComponent()

    RegisterSubscription(EntityComponentRemoved, OnBgfxIndexBufferRemoved, ComponentOf_BgfxIndexBuffer())
    RegisterSubscription(EntityComponentRemoved, OnBgfxVertexBufferRemoved, ComponentOf_BgfxVertexBuffer())
    RegisterSubscription(Validate, OnValidation, 0)
EndUnit()