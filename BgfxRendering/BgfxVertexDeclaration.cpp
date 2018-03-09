//
// Created by Kim Johannsen on 09-03-2018.
//

#include <Core/Entity.h>
#include "BgfxVertexDeclaration.h"
#include <bgfx/bgfx.h>
#include <Rendering/VertexAttribute.h>
#include <Rendering/VertexBuffer.h>
#include <Rendering/VertexDeclaration.h>

struct BgfxVertexDeclaration {
    BgfxVertexDeclaration() : invalidated(true) {}

    bgfx::VertexDecl decl;
    bool invalidated;
};

DefineComponent(BgfxVertexDeclaration)
EndComponent()

DefineService(BgfxVertexDeclaration)
EndService()

static void OnChanged(Entity entity) {
    if(HasBgfxVertexDeclaration(entity)) {
        GetBgfxVertexDeclaration(entity)->invalidated = true;
    }
}

static bool ServiceStart() {
    SubscribeVertexDeclarationChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeVertexDeclarationChanged(OnChanged);
    return true;
}

void* GetBgfxVertexDeclarationHandle(Entity entity) {
    auto data = GetBgfxVertexDeclaration(entity);
    if(data->invalidated) {
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

        for_entity(vertexBuffer, VertexBuffer) {
            if(GetVertexBufferDeclaration(vertexBuffer) == entity) {
                FireEvent(VertexBufferChanged, vertexBuffer);
            }
        }
    }
    return &data->decl;
}