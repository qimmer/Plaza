//
// Created by Kim on 21-08-2018.
//

#include <Core/Debug.h>
#include <Rendering/Mesh.h>
#include <Rendering/RenderingModule.h>
#include "BgfxMesh.h"
#include "BgfxResource.h"

#include <bgfx/bgfx.h>
#include <Foundation/Stream.h>
#include <Rendering/RenderContext.h>
#include <Foundation/AppLoop.h>

static eastl::set<Entity> invalidatedVertexBuffers, invalidatedIndexBuffers, invalidatedVertexDecls;

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

static void ValidateVertexBuffer(Entity entity) {
    auto data = GetBgfxVertexBuffer(entity);
    auto decl = GetVertexBuffer(entity).VertexBufferDeclaration;
    auto declData = GetBgfxVertexDeclaration(decl);
    auto vertexBufferData = GetVertexBuffer(entity);

    if(declData.decl.m_hash == 0) return;

    if(!StreamOpen(entity, StreamMode_Read)) return;

    StreamSeek(entity, StreamSeek_End);
    auto size = StreamTell(entity);

    if(size == 0) {
        StreamClose(entity);
        return;
    }

    auto buffer = alloca(size);
    StreamSeek(entity, 0);
    StreamRead(entity, size, buffer);
    StreamClose(entity);

    if(vertexBufferData.VertexBufferDynamic) {
        bgfx::DynamicVertexBufferHandle dynHandle = { GetBgfxResource(entity).BgfxResourceHandle };

        if(dynHandle.idx != bgfx::kInvalidHandle || !data.dynamic || data.declHash != declData.decl.m_hash) {
            if(bgfx::isValid(dynHandle)) {
                bgfx::destroy(dynHandle);
            }

            SetBgfxResource(entity, {bgfx::createDynamicVertexBuffer(bgfx::copy(buffer, size), declData.decl).idx});
        } else {
            bgfx::update(dynHandle, 0, bgfx::copy(buffer, size));
        }

        data.dynamic = true;
    } else {
        bgfx::VertexBufferHandle handle = { GetBgfxResource(entity).BgfxResourceHandle };

        if(bgfx::isValid(handle)) {
            bgfx::destroy(handle);
        }

        SetBgfxResource(entity, {bgfx::createVertexBuffer(bgfx::copy(buffer, size), declData.decl).idx});

        data.dynamic = false;
    }

    data.size = size;
    data.declHash = declData.decl.m_hash;
    SetBgfxVertexBuffer(entity, data);
}

static void ValidateIndexBuffer(Entity entity) {
    auto data = GetBgfxIndexBuffer(entity);
    auto indexBufferData = GetIndexBuffer(entity);

    if(!StreamOpen(entity, StreamMode_Read)) return;

    StreamSeek(entity, StreamSeek_End);
    auto size = StreamTell(entity);

    if(size == 0) {
        StreamClose(entity);
        return;
    }

    auto buffer = alloca(size);
    StreamSeek(entity, 0);
    StreamRead(entity, size, buffer);
    StreamClose(entity);

    if(indexBufferData.IndexBufferDynamic) {
        bgfx::DynamicIndexBufferHandle dynHandle = { GetBgfxResource(entity).BgfxResourceHandle };

        if(dynHandle.idx != bgfx::kInvalidHandle || !data.dynamic || data.isLong != indexBufferData.IndexBufferLong) {
            if(bgfx::isValid(dynHandle)) {
                bgfx::destroy(dynHandle);
            }

            SetBgfxResource(entity, {bgfx::createDynamicIndexBuffer(bgfx::copy(buffer, size), indexBufferData.IndexBufferLong ? BGFX_BUFFER_INDEX32 : BGFX_BUFFER_NONE).idx});
        } else {
            bgfx::update(dynHandle, 0, bgfx::copy(buffer, size));
        }

        data.dynamic = true;
    } else {
        bgfx::IndexBufferHandle handle = { GetBgfxResource(entity).BgfxResourceHandle };

        if(bgfx::isValid(handle)) {
            bgfx::destroy(handle);
        }

        SetBgfxResource(entity, {bgfx::createIndexBuffer(bgfx::copy(buffer, size), indexBufferData.IndexBufferLong ? BGFX_BUFFER_INDEX32 : BGFX_BUFFER_NONE).idx});

        data.dynamic = false;
    }

    data.size = size;
    data.isLong = indexBufferData.IndexBufferLong;
    SetBgfxIndexBuffer(entity, data);
}

static void ValidateVertexDeclaration(Entity entity) {
    auto data = GetBgfxVertexDeclaration(entity);
    auto vertexDeclarationData = GetVertexDeclaration(entity);

    data.decl.begin();
    for(auto attribute : vertexDeclarationData.VertexDeclarationAttributes) {
        auto attribData = GetVertexAttribute(attribute);
        bgfx::AttribType::Enum elementType;
        u32 elementCount;
        if(attribData.VertexAttributeType == TypeOf_float) {
            elementType = bgfx::AttribType::Float;
            elementCount = 1;
        } else if(attribData.VertexAttributeType == TypeOf_v2f) {
            elementType = bgfx::AttribType::Float;
            elementCount = 2;
        } else if(attribData.VertexAttributeType == TypeOf_v3f) {
            elementType = bgfx::AttribType::Float;
            elementCount = 3;
        } else if(attribData.VertexAttributeType == TypeOf_v4f) {
            elementType = bgfx::AttribType::Float;
            elementCount = 4;
        } else if(attribData.VertexAttributeType == TypeOf_rgba8) {
            elementType = bgfx::AttribType::Uint8;
            elementCount = 4;
        } else if(attribData.VertexAttributeType == TypeOf_s16) {
            elementType = bgfx::AttribType::Int16;
            elementCount = 1;
        } else {
            Log(entity, LogSeverity_Error, "Unsupported attribute type: %s", GetTypeName(attribData.VertexAttributeType));
            continue;
        }

        auto elementUsage = (bgfx::Attrib::Enum)attribData.VertexAttributeUsage;
        data.decl.add(
                elementUsage,
                elementCount,
                elementType,
                attribData.VertexAttributeNormalize,
                attribData.VertexAttributeAsInt);
    }

    data.decl.end();

    SetBgfxVertexDeclaration(entity, data);
}

static void InvalidateVertexBuffer(Entity vb) {
    invalidatedVertexBuffers.insert(vb);
}

static void InvalidateIndexBuffer(Entity ib) {
    invalidatedIndexBuffers.insert(ib);
}

static void InvalidateAttribute(Entity attr) {
    auto decl = GetOwnership(attr).Owner;
    invalidatedVertexDecls.insert(decl);

    VertexBuffer data;
    for_entity_data(vertexBuffer, ComponentOf_BgfxVertexBuffer(), &data) {
        if(data.VertexBufferDeclaration == decl) {
            InvalidateVertexBuffer(vertexBuffer);
        }
    }
}

static void OnBgfxResourceChanged(Entity entity, const BgfxResource& oldData, const BgfxResource& newData) {
    if(oldData.BgfxResourceHandle != bgfx::kInvalidHandle && newData.BgfxResourceHandle != oldData.BgfxResourceHandle) {
        if(HasComponent(entity, ComponentOf_BgfxVertexBuffer())) {
            bgfx::DynamicVertexBufferHandle dynHandle = { oldData.BgfxResourceHandle };
            bgfx::VertexBufferHandle staHandle = { dynHandle.idx };

            if(GetBgfxVertexBuffer(entity).dynamic) {
                if(bgfx::isValid(dynHandle)) {
                    bgfx::destroy(dynHandle);
                }
            } else {
                if(bgfx::isValid(staHandle)) {
                    bgfx::destroy(staHandle);
                }
            }
        }

        if(HasComponent(entity, ComponentOf_BgfxIndexBuffer())) {
            bgfx::DynamicIndexBufferHandle dynHandle = { oldData.BgfxResourceHandle };
            bgfx::IndexBufferHandle staHandle = { dynHandle.idx };

            if(GetBgfxIndexBuffer(entity).dynamic) {
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
}

static void OnAppLoopChanged(Entity entity, const AppLoop& oldData, const AppLoop& newData) {
    for(auto& entity : invalidatedVertexDecls) {
        ValidateVertexDeclaration(entity);
    }
    invalidatedVertexDecls.clear();

    for(auto& entity : invalidatedVertexBuffers) {
        ValidateVertexBuffer(entity);
    }

    invalidatedVertexBuffers.clear();
    for(auto& entity : invalidatedIndexBuffers) {
        ValidateIndexBuffer(entity);
    }
    invalidatedIndexBuffers.clear();
}

static void OnStreamChanged(Entity entity, const Stream& oldData, const Stream& newData) {
    if(HasComponent(entity, ComponentOf_BgfxVertexBuffer())) {
        invalidatedVertexBuffers.insert(entity);
    }

    if(HasComponent(entity, ComponentOf_BgfxIndexBuffer())) {
        invalidatedIndexBuffers.insert(entity);
    }
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

    RegisterSystem(OnBgfxResourceChanged, ComponentOf_BgfxResource())
    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_ResourceSubmission)
    RegisterSystem(OnStreamChanged, ComponentOf_Stream())
EndUnit()