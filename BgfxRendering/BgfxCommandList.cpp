//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Rendering/CommandList.h>
#include <bgfx/bgfx.h>
#include <Core/Hierarchy.h>
#include <Rendering/Batch.h>
#include <Rendering/Uniform.h>
#include <Rendering/VertexBuffer.h>
#include <Rendering/IndexBuffer.h>
#include <Rendering/Material.h>
#include <Rendering/Mesh.h>
#include <Rendering/UniformState.h>
#include <Core/Types.h>
#include <Foundation/Visibility.h>
#include <Rendering/SubTexture2D.h>
#include <Rendering/Texture2D.h>
#include <Rendering/BinaryShader.h>
#include "BgfxCommandList.h"
#include "BgfxVertexBuffer.h"
#include "BgfxIndexBuffer.h"
#include "BgfxProgram.h"
#include "BgfxUniform.h"
#include "BgfxTexture2D.h"


struct BgfxCommandList {
};

DefineComponent(BgfxCommandList)
EndComponent()

DefineService(BgfxCommandList)
EndService()

unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA)
{
    return (((((iR << 8) + iG) << 8) + iB) << 8) + iA;
}

static void SetUniformState(Entity uniformState) {
    auto uniform = GetUniformStateUniform(uniformState);
    if(IsEntityValid(uniform) && HasUniform(uniform)) {
        auto uniformType = GetUniformType(uniform);
        auto handle = bgfx::UniformHandle{GetBgfxUniformHandle(uniform)};

        if(IsTypeValid(uniformType)) {
            if(uniformType != TypeOf_Entity()) {
                auto state = GetUniformStateMat4(uniformState);
                bgfx::setUniform(
                        handle,
                        &state,
                        GetUniformArrayCount(uniform));
            } else {
                auto texture = GetUniformStateTexture(uniformState);
                if(IsEntityValid(texture)) {
                    auto textureParent = GetParent(texture);
                    auto uvOffsetScaleUniform = GetBgfxUniformHandle(GetSubTexture2DUvOffsetScaleUniform());
                    if(HasSubTexture2D(texture) && IsEntityValid(textureParent) && HasTexture2D(textureParent)) {
                        auto size = GetTextureSize2D(textureParent);
                        auto uvOffset = GetSubTexture2DOffset(texture);
                        auto uvSize = GetSubTexture2DSize(texture);
                        v4f uvOffsetScale = {
                                (float)uvOffset.x / size.x,
                                (float)uvOffset.y / size.y,
                                (float)uvSize.x / size.x,
                                (float)uvSize.y / size.y
                        };
                        bgfx::setUniform(bgfx::UniformHandle{uvOffsetScaleUniform}, &uvOffsetScale.x);

                        texture = textureParent;
                    } else {
                        v4f uvOffsetScale = {
                                0.0f,
                                0.0f,
                                1.0f,
                                1.0f
                        };
                        bgfx::setUniform(bgfx::UniformHandle{uvOffsetScaleUniform}, &uvOffsetScale.x);
                    }

                    if(HasBgfxTexture2D(texture)) {
                        bgfx::setTexture(GetUniformStateStage(uniformState), handle, bgfx::TextureHandle{GetBgfxTexture2DHandle(texture)});
                    }
                }
            }
        }
    }
}

void RenderBatch(u32 viewId, Entity entity, u8 shaderProfile) {
    auto scissor = GetBatchScissor(entity);
    auto transform = GetBatchWorldMatrix(entity);
    auto mesh = GetBatchMesh(entity);
    auto material = GetBatchMaterial(entity);

    if(!IsEntityValid(mesh) || !IsEntityValid(material)) return;

    auto program = GetMaterialProgram(material);
    auto vertexBuffer = GetMeshVertexBuffer(mesh);
    auto indexBuffer = GetMeshIndexBuffer(mesh);

    if(!IsEntityValid(program) ||
       !IsEntityValid(vertexBuffer) ||
       !IsEntityValid(indexBuffer) ||
       !IsEntityValid(mesh) ||
       !IsEntityValid(material)) {
        return;
    }

    auto vertexDeclaration = GetVertexBufferDeclaration(vertexBuffer);

    auto programHandle = GetBgfxProgramHandle(program, shaderProfile);
    if(GetBgfxVertexBufferHandle(vertexBuffer) == bgfx::kInvalidHandle ||
       GetBgfxIndexBufferHandle(indexBuffer) == bgfx::kInvalidHandle ||
            programHandle == bgfx::kInvalidHandle) return;

    bgfx::setState(GetMaterialRenderState(material) | GetMeshPrimitiveType(mesh));

    if(scissor.x < scissor.z && scissor.y < scissor.w) {
        bgfx::setScissor(scissor.x, scissor.y, scissor.z, scissor.w);
    }

    bgfx::setTransform(&transform);

    if(GetVertexBufferDynamic(vertexBuffer)) {
        bgfx::setVertexBuffer(0, bgfx::DynamicVertexBufferHandle {GetBgfxVertexBufferHandle(vertexBuffer)}, GetMeshStartVertex(mesh), GetMeshNumVertices(mesh));
    } else {
        bgfx::setVertexBuffer(0, bgfx::VertexBufferHandle {GetBgfxVertexBufferHandle(vertexBuffer)}, GetMeshStartVertex(mesh), GetMeshNumVertices(mesh));
    }

    if(GetIndexBufferDynamic(indexBuffer)) {
        bgfx::setIndexBuffer(bgfx::DynamicIndexBufferHandle {GetBgfxIndexBufferHandle(indexBuffer)}, GetMeshStartIndex(mesh), GetMeshNumIndices(mesh));
    } else {
        bgfx::setIndexBuffer(bgfx::IndexBufferHandle {GetBgfxIndexBufferHandle(indexBuffer)}, GetMeshStartIndex(mesh), GetMeshNumIndices(mesh));
    }

    for(auto uniformState = GetFirstChild(material); IsEntityValid(uniformState); uniformState = GetSibling(uniformState)) {
        if(!HasUniformState(uniformState)) continue;

        SetUniformState(uniformState);
    }

    bgfx::submit(viewId, bgfx::ProgramHandle {programHandle});
}

void RenderCommandList(Entity entity, unsigned char viewId) {
    if(HasVisibility(entity) && GetHidden(entity)) return;

    auto viewport = GetCommandListViewport(entity);

    auto clearColor = GetCommandListClearColor(entity);
    auto viewMat = GetCommandListViewMatrix(entity);
    auto projMat = GetCommandListProjectionMatrix(entity);

    //bgfx::setViewFrameBuffer(viewId, )

    u8 shaderProfile;
    switch(bgfx::getRendererType()) {
        case bgfx::RendererType::Direct3D9:
            shaderProfile = ShaderProfile_HLSL_3_0;
            break;
        case bgfx::RendererType::Direct3D11:
            shaderProfile = ShaderProfile_HLSL_5_0;
            break;
        case bgfx::RendererType::Direct3D12:
            shaderProfile = ShaderProfile_HLSL_5_0;
            break;
        case bgfx::RendererType::OpenGL:
            shaderProfile = ShaderProfile_GLSL_4_1;
            break;
        case bgfx::RendererType::Metal:
            shaderProfile = ShaderProfile_Metal_OSX;
            break;
        default:
            Log(LogChannel_Core, LogSeverity_Error, "Unsupported renderer type");
            return;
            break;
    }
    bgfx::setViewClear(
            viewId,
            GetCommandListClearTargets(entity),
            RGBA2DWORD(clearColor.r, clearColor.g, clearColor.b, clearColor.a),
            GetCommandListClearDepth(entity),
            GetCommandListClearStencil(entity));

    bgfx::setViewRect(viewId, viewport.x, viewport.y, viewport.z, viewport.w);
    bgfx::setViewTransform(viewId, &viewMat, &projMat);

    bgfx::touch(viewId);

    for(auto batch = GetFirstChild(entity); IsEntityValid(batch); batch = GetSibling(batch)) {
        if(!HasBatch(batch)) continue;

        RenderBatch(viewId, batch, shaderProfile);
    }
}

static bool ServiceStart() {

    return true;
}

static bool ServiceStop() {

    return true;
}
