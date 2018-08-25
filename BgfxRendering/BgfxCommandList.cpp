//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Rendering/CommandList.h>
#include <bgfx/bgfx.h>
#include <Rendering/Batch.h>
#include <Rendering/Uniform.h>
#include <Rendering/Mesh.h>
#include <Rendering/Material.h>
#include <Core/Types.h>
#include <Foundation/Visibility.h>
#include <Rendering/SubTexture2D.h>
#include <Rendering/Texture2D.h>
#include <Rendering/ShaderCache.h>
#include <Rendering/RenderContext.h>
#include <Rendering/Program.h>
#include <Rendering/RenderingModule.h>
#include <Core/Math.h>
#include "BgfxCommandList.h"
#include "BgfxMesh.h"
#include "BgfxShaderCache.h"
#include "BgfxUniform.h"
#include "BgfxTexture2D.h"
#include "BgfxRenderContext.h"
#include "BgfxOffscreenRenderTarget.h"
#include "BgfxResource.h"

struct BgfxCommandList {
};

static void SetUniformState(Entity uniformState) {
    static m4x4f state[256];

    auto uniform = GetUniformStateUniform(uniformState);
    if(HasComponent(uniform, ComponentOf_Uniform())) {
        auto uniformType = GetUniformType(uniform);
        auto typeSize = GetTypeSize(uniformType);

        if(uniformType <= TypeOf_void) return;

        auto uniformSize = GetTypeSize(uniformType);
        auto uniformArrayCount = GetUniformArrayCount(uniform);

        auto handle = bgfx::UniformHandle{GetBgfxResourceHandle(uniform)};

        if(uniformType != TypeOf_Entity) {
            auto ptr = (char*)&state;

            for_children(element, UniformStateElements, uniformState) {
                memcpy(ptr, GetUniformStateElementData(element), typeSize);
                ptr += typeSize;
            }

            bgfx::setUniform(
                    handle,
                    &state,
                    GetNumUniformStateElements(uniformState));
        } else {
            for_children(element, UniformStateElements, uniformState) {
                auto texture = GetUniformStateElementTexture(element);

                if(IsEntityValid(texture)) {
                    auto uvOffsetScaleUniform = GetBgfxResourceHandle(GetRenderingUvOffsetScaleUniform(ModuleOf_Rendering()));
                    Entity textureParent = 0;
                    if(HasComponent(texture, ComponentOf_SubTexture2D()) && (textureParent = GetSubTexture2DTexture(texture)) && IsEntityValid(textureParent)) {
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

                    if(HasComponent(texture, ComponentOf_BgfxTexture2D())) {
                        bgfx::setTexture(i, handle, bgfx::TextureHandle{GetBgfxResourceHandle(texture)});
                    }
                }
            }
        }
    }
}

void RenderBatch(u32 viewId, Entity entity) {
    auto scissor = GetBatchScissor(entity);
    auto transform = GetBatchWorldMatrix(entity);
    auto subMesh = GetBatchSubMesh(entity);
    auto material = GetBatchMaterial(entity);
    auto program = GetBatchBinaryProgram(entity);

    if(!IsEntityValid(program) || !IsEntityValid(subMesh) || !IsEntityValid(material)) return;

    auto mesh = GetOwner(subMesh);
    auto vertexBuffer = GetMeshVertexBuffer(mesh);
    auto indexBuffer = GetMeshIndexBuffer(mesh);
    auto vertexDeclaration = GetVertexBufferDeclaration(vertexBuffer);

    auto programHandle = GetBgfxResourceHandle(program);

    if(GetBgfxResourceHandle(vertexBuffer) == bgfx::kInvalidHandle ||
       programHandle == bgfx::kInvalidHandle ||
            (IsEntityValid(indexBuffer) && GetBgfxResourceHandle(indexBuffer) == bgfx::kInvalidHandle )) return;

    bgfx::setState(GetMaterialWriteMask(material) |
                           GetMaterialBlendMode(material) |
                           GetMaterialDepthTest(material) |
                           GetMaterialMultisampleMode(material) |
                           GetSubMeshCullMode(subMesh) |
                           GetSubMeshPrimitiveType(subMesh));

    if(scissor.z > 0 && scissor.w > 0) {
        bgfx::setScissor(scissor.x, scissor.y, scissor.z, scissor.w);
    }

    bgfx::setTransform(&transform);

    if(GetVertexBufferDynamic(vertexBuffer)) {
        bgfx::setVertexBuffer(0, bgfx::DynamicVertexBufferHandle {GetBgfxResourceHandle(vertexBuffer)}, GetSubMeshStartVertex(subMesh), GetSubMeshNumVertices(subMesh));
    } else {
        bgfx::setVertexBuffer(0, bgfx::VertexBufferHandle {GetBgfxResourceHandle(vertexBuffer)}, GetSubMeshStartVertex(subMesh), GetSubMeshNumVertices(subMesh));
    }

    if(IsEntityValid(indexBuffer)) {
        if(GetIndexBufferDynamic(indexBuffer)) {
            bgfx::setIndexBuffer(bgfx::DynamicIndexBufferHandle {GetBgfxResourceHandle(indexBuffer)}, GetSubMeshStartIndex(subMesh), GetSubMeshNumIndices(subMesh));
        } else {
            bgfx::setIndexBuffer(bgfx::IndexBufferHandle {GetBgfxResourceHandle(indexBuffer)}, GetSubMeshStartIndex(subMesh), GetSubMeshNumIndices(subMesh));
        }
    }

    {
        for_children(uniformState, MaterialUniformStates, material) {
            SetUniformState(uniformState);
        }
    }

    {
        for_children(uniformState, BatchUniformStates, entity) {
            SetUniformState(uniformState);
        }
    }

    bgfx::submit(viewId, bgfx::ProgramHandle {programHandle});
}

void RenderCommandList(Entity entity, unsigned char viewId) {
    if(HasComponent(entity, ComponentOf_Visibility()) && GetHidden(entity)) return;

    auto viewport = GetCommandListViewport(entity);

    auto clearColor = GetCommandListClearColor(entity);
    auto viewMat = GetCommandListViewMatrix(entity);
    auto projMat = GetCommandListProjectionMatrix(entity);

    auto renderTarget = GetCommandListRenderTarget(entity);
    if(IsEntityValid(renderTarget)) {
        if(HasComponent(renderTarget, ComponentOf_BgfxRenderContext())) {
            bgfx::FrameBufferHandle fb = {GetBgfxResourceHandle(renderTarget)};
            if(fb.idx != bgfx::kInvalidHandle) {
                bgfx::setViewFrameBuffer(viewId, fb);
            }

        }

        if(HasComponent(renderTarget, ComponentOf_BgfxOffscreenRenderTarget())) {
            bgfx::FrameBufferHandle fb = {GetBgfxResourceHandle(renderTarget)};
            if(fb.idx != bgfx::kInvalidHandle) {
                bgfx::setViewFrameBuffer(viewId, fb);
            }
        }
    }

    u8 shaderProfile;
    auto rendererType = bgfx::getRendererType();
    switch(rendererType) {
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
            shaderProfile = ShaderProfile_GLSL_2_1;
            break;
        case bgfx::RendererType::Metal:
            shaderProfile = ShaderProfile_Metal_OSX;
            break;
        default:
            return;
            break;
    }

    SetShaderCacheProfile(GetCommandListShaderCache(entity), shaderProfile);

    bgfx::setViewClear(
            viewId,
            GetCommandListClearTargets(entity),
            RGBA2DWORD(clearColor.r, clearColor.g, clearColor.b, clearColor.a),
            GetCommandListClearDepth(entity),
            GetCommandListClearStencil(entity));

    bgfx::setViewRect(viewId, viewport.x, viewport.y, viewport.z, viewport.w);
    bgfx::setViewTransform(viewId, &viewMat, &projMat);

    bgfx::touch(viewId);


    {
        for_children(uniformState, CommandListUniformStates, entity) {
            SetUniformState(uniformState);
        }
    }

    {
        for_children(batch, CommandListBatches, entity) {
            RenderBatch(viewId, batch);
        }
    }
}

BeginUnit(BgfxCommandList)
    BeginComponent(BgfxCommandList)
    EndComponent()
EndUnit()