//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Rendering/CommandList.h>
#include <bgfx/bgfx.h>
#include <Core/Node.h>
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
#include <Rendering/Context.h>
#include <Rendering/Program.h>
#include "BgfxCommandList.h"
#include "BgfxVertexBuffer.h"
#include "BgfxIndexBuffer.h"
#include "BgfxProgram.h"
#include "BgfxUniform.h"
#include "BgfxTexture2D.h"
#include "BgfxContext.h"
#include "BgfxOffscreenRenderTarget.h"


struct BgfxCommandList {
};

BeginUnit(BgfxCommandList)
    BeginComponent(BgfxCommandList)
EndComponent()

unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA)
{
    return (((((iR << 8) + iG) << 8) + iB) << 8) + iA;
}

static void SetUniformState(Entity uniformState) {
    static Type typeOfEntity = TypeOf_Entity();
    static m4x4f state[32];

    auto uniform = GetUniformStateUniform(uniformState);
    if(IsEntityValid(uniform) && HasComponent(uniform, ComponentOf_Uniform())) {
        auto uniformType = GetUniformType(uniform);

        if(IsTypeValid(uniformType)) {
            auto uniformSize = GetTypeSize(uniformType);
            auto uniformArrayCount = GetUniformArrayCount(uniform);

            auto handle = bgfx::UniformHandle{GetBgfxUniformHandle(uniform)};

            if(uniformType != typeOfEntity) {
                bgfx::setUniform(
                        handle,
                        GetUniformStateStateRaw(uniformState),
                        uniformArrayCount);
            } else {
                auto texture = GetUniformStateTexture(uniformState);
                if(IsEntityValid(texture)) {
                    auto uvOffsetScaleUniform = GetBgfxUniformHandle(GetSubTexture2DUvOffsetScaleUniform());
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

    auto program = ResolveProgram(GetMaterialVertexShader(material), GetMaterialPixelShader(material), shaderProfile, GetBatchShaderVariation(entity));
    auto vertexBuffer = GetMeshVertexBuffer(mesh);
    auto indexBuffer = GetMeshIndexBuffer(mesh);

    if(!IsEntityValid(program) ||
       !IsEntityValid(vertexBuffer) ||
       !IsEntityValid(mesh) ||
       !IsEntityValid(material)) {
        return;
    }

    auto vertexDeclaration = GetVertexBufferDeclaration(vertexBuffer);

    auto programHandle = GetBgfxProgramHandle(program);
    if(GetBgfxVertexBufferHandle(vertexBuffer) == bgfx::kInvalidHandle ||
       programHandle == bgfx::kInvalidHandle ||
            (IsEntityValid(indexBuffer) && GetBgfxIndexBufferHandle(indexBuffer) == bgfx::kInvalidHandle )) return;

    bgfx::setState(GetMaterialWriteMask(material) |
                           GetMaterialBlendMode(material) |
                           GetMaterialDepthTest(material) |
                           GetMaterialMultisampleMode(material) |
                           GetMeshCullMode(mesh) |
                           GetMeshPrimitiveType(mesh));

    if(scissor.z > 0 && scissor.w > 0) {
        bgfx::setScissor(scissor.x, scissor.y, scissor.z, scissor.w);
    }

    bgfx::setTransform(&transform);

    if(GetVertexBufferDynamic(vertexBuffer)) {
        bgfx::setVertexBuffer(0, bgfx::DynamicVertexBufferHandle {GetBgfxVertexBufferHandle(vertexBuffer)}, GetMeshStartVertex(mesh), GetMeshNumVertices(mesh));
    } else {
        bgfx::setVertexBuffer(0, bgfx::VertexBufferHandle {GetBgfxVertexBufferHandle(vertexBuffer)}, GetMeshStartVertex(mesh), GetMeshNumVertices(mesh));
    }

    if(IsEntityValid(indexBuffer)) {
        if(GetIndexBufferDynamic(indexBuffer)) {
            bgfx::setIndexBuffer(bgfx::DynamicIndexBufferHandle {GetBgfxIndexBufferHandle(indexBuffer)}, GetMeshStartIndex(mesh), GetMeshNumIndices(mesh));
        } else {
            bgfx::setIndexBuffer(bgfx::IndexBufferHandle {GetBgfxIndexBufferHandle(indexBuffer)}, GetMeshStartIndex(mesh), GetMeshNumIndices(mesh));
        }
    }

    for(auto uniformState = GetFirstChild(material); uniformState; uniformState = GetSibling(uniformState)) {
        if(!HasComponent(uniformState, ComponentOf_UniformState())) continue;

        SetUniformState(uniformState);
    }

    for(auto uniformState = GetFirstChild(program); uniformState; uniformState = GetSibling(uniformState)) {
        if(!HasComponent(uniformState, ComponentOf_UniformState())) continue;

        SetUniformState(uniformState);
    }

    for(auto uniformState = GetFirstChild(entity); uniformState; uniformState = GetSibling(uniformState)) {
        if(!HasComponent(uniformState, ComponentOf_UniformState())) continue;

        SetUniformState(uniformState);
    }

    bgfx::submit(viewId, bgfx::ProgramHandle {programHandle});
}

void RenderCommandList(Entity entity, unsigned char viewId) {
    if(HasComponent(entity, ComponentOf_Visibility()) && GetHidden(entity)) return;

    for_entity(uniform, data, Uniform) {
        if(!HasComponent(uniform, ComponentOf_UniformState())) continue;

        SetUniformState(uniform);
    }

    auto viewport = GetCommandListViewport(entity);

    auto clearColor = GetCommandListClearColor(entity);
    auto viewMat = GetCommandListViewMatrix(entity);
    auto projMat = GetCommandListProjectionMatrix(entity);

    auto renderTarget = GetCommandListRenderTarget(entity);
    if(IsEntityValid(renderTarget)) {
        if(HasComponent(renderTarget, ComponentOf_BgfxContext())) {
            bgfx::FrameBufferHandle fb = {GetBgfxContextHandle(renderTarget)};
            if(fb.idx != bgfx::kInvalidHandle) {
                bgfx::setViewFrameBuffer(viewId, fb);
            }

        }

        if(HasComponent(renderTarget, ComponentOf_BgfxOffscreenRenderTarget())) {
            bgfx::FrameBufferHandle fb = {GetBgfxOffscreenRenderTargetHandle(renderTarget)};
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
    bgfx::setViewClear(
            viewId,
            GetCommandListClearTargets(entity),
            RGBA2DWORD(clearColor.r, clearColor.g, clearColor.b, clearColor.a),
            GetCommandListClearDepth(entity),
            GetCommandListClearStencil(entity));

    bgfx::setViewRect(viewId, viewport.x, viewport.y, viewport.z, viewport.w);
    bgfx::setViewTransform(viewId, &viewMat, &projMat);

    bgfx::touch(viewId);

    for(auto uniformState = GetFirstChild(entity); uniformState; uniformState = GetSibling(uniformState)) {
        if(!HasComponent(uniformState, ComponentOf_UniformState())) continue;

        SetUniformState(uniformState);
    }

    for(auto batch = GetFirstChild(entity); IsEntityValid(batch); batch = GetSibling(batch)) {
        if(!HasComponent(batch, ComponentOf_Batch())) continue;

        RenderBatch(viewId, batch, shaderProfile);
    }
}
