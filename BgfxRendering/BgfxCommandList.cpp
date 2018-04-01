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

DefineComponent(BgfxCommandList)
EndComponent()

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
                m4x4f state[32];
                GetUniformStateState(uniformState, sizeof(m4x4f)*32, state);
                bgfx::setUniform(
                        handle,
                        &state,
                        GetUniformArrayCount(uniform));
            } else {
                auto texture = GetUniformStateTexture(uniformState);
                if(IsEntityValid(texture)) {
                    auto uvOffsetScaleUniform = GetBgfxUniformHandle(GetSubTexture2DUvOffsetScaleUniform());
                    Entity textureParent = 0;
                    if(HasSubTexture2D(texture) && (textureParent = GetSubTexture2DTexture(texture)) && IsEntityValid(textureParent)) {
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
        if(!HasUniformState(uniformState)) continue;

        SetUniformState(uniformState);
    }

    for(auto uniformState = GetFirstChild(program); uniformState; uniformState = GetSibling(uniformState)) {
        if(!HasUniformState(uniformState)) continue;

        SetUniformState(uniformState);
    }

    for(auto uniformState = GetFirstChild(entity); uniformState; uniformState = GetSibling(uniformState)) {
        if(!HasUniformState(uniformState)) continue;

        SetUniformState(uniformState);
    }

    bgfx::submit(viewId, bgfx::ProgramHandle {programHandle});
}

void RenderCommandList(Entity entity, unsigned char viewId) {
    if(HasVisibility(entity) && GetHidden(entity)) return;

    for_entity(uniform, Uniform) {
        if(!HasUniformState(uniform)) continue;

        SetUniformState(uniform);
    }

    auto viewport = GetCommandListViewport(entity);

    auto clearColor = GetCommandListClearColor(entity);
    auto viewMat = GetCommandListViewMatrix(entity);
    auto projMat = GetCommandListProjectionMatrix(entity);

    auto renderTarget = GetCommandListRenderTarget(entity);
    if(IsEntityValid(renderTarget)) {
        if(HasBgfxContext(renderTarget)) {
            bgfx::FrameBufferHandle fb = {GetBgfxContextHandle(renderTarget)};
            if(fb.idx != bgfx::kInvalidHandle) {
                bgfx::setViewFrameBuffer(viewId, fb);
            }

        }

        if(HasBgfxOffscreenRenderTarget(renderTarget)) {
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

    for(auto uniformState = GetFirstChild(entity); uniformState; uniformState = GetSibling(uniformState)) {
        if(!HasUniformState(uniformState)) continue;

        SetUniformState(uniformState);
    }

    for(auto batch = GetFirstChild(entity); IsEntityValid(batch); batch = GetSibling(batch)) {
        if(!HasBatch(batch)) continue;

        RenderBatch(viewId, batch, shaderProfile);
    }
}
