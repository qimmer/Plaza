//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/Algorithms.h>

#include <Foundation/Visibility.h>

#include <Rendering/Uniform.h>
#include <Rendering/Mesh.h>
#include <Rendering/Material.h>
#include <Rendering/SubTexture2D.h>
#include <Rendering/Texture2D.h>
#include <Rendering/ShaderCache.h>
#include <Rendering/RenderContext.h>
#include <Rendering/Program.h>
#include <Rendering/RenderState.h>
#include <Rendering/SceneRenderer.h>
#include <Rendering/Renderable.h>
#include <Rendering/RenderingModule.h>

#include <Scene/Transform.h>
#include <Scene/Frustum.h>
#include <Scene/Camera.h>

#include "BgfxCommandList.h"
#include "BgfxMesh.h"
#include "BgfxShaderCache.h"
#include "BgfxUniform.h"
#include "BgfxTexture2D.h"
#include "BgfxRenderContext.h"
#include "BgfxOffscreenRenderTarget.h"
#include "BgfxResource.h"

#include <bgfx/bgfx.h>
#include <Rendering/RenderTarget.h>
#include <Foundation/Invalidation.h>

struct BgfxCommandList {
};

static void SetUniformState(Entity uniform, Entity entity) {
    /*static m4x4f state[256];

    auto uniformType = GetPropertyType(uniform);
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
    }*/
}

void RenderBatch(u32 viewId, Entity batch, Entity renderState, Entity pass) {
    auto renderable = GetBatchRenderable(batch);
    auto binaryProgram = GetBatchBinaryProgram(batch);

    if(!HasComponent(renderable, ComponentOf_Renderable()) || !HasComponent(binaryProgram, ComponentOf_BinaryProgram())) return;

    auto worldMatrix = GetTransformGlobalMatrix(renderable);

    auto subMesh = GetRenderableSubMesh(renderable);
    auto material = GetRenderableMaterial(renderable);

    if(!IsEntityValid(binaryProgram) || !IsEntityValid(subMesh) || !IsEntityValid(material)) return;

    auto mesh = GetOwner(subMesh);
    auto vertexBuffer = GetMeshVertexBuffer(mesh); Validate(vertexBuffer);
    auto indexBuffer = GetMeshIndexBuffer(mesh); Validate(indexBuffer);
    auto vertexDeclaration = GetVertexBufferDeclaration(vertexBuffer); Validate(vertexDeclaration);
    Validate(mesh);

    auto programHandle = GetBgfxResourceHandle(binaryProgram); Validate(binaryProgram);

    if(GetBgfxResourceHandle(vertexBuffer) == bgfx::kInvalidHandle) return;
    if(programHandle == bgfx::kInvalidHandle) return;
    if(IsEntityValid(indexBuffer) && GetBgfxResourceHandle(indexBuffer) == bgfx::kInvalidHandle) return;

    bgfx::setState(
            GetRenderStateWriteMask(renderState) |
       GetRenderStateBlendMode(renderState) |
       GetRenderStateDepthTest(renderState) |
       GetRenderStateMultisampleMode(renderState) |
       GetSubMeshCullMode(subMesh) |
       GetSubMeshPrimitiveType(subMesh)
    );

    bgfx::setTransform(&worldMatrix);

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
        for_children(uniform, RenderPassMaterialUniforms, pass) {
            SetUniformState(uniform, material);
        }
    }

    {
        for_children(uniform, RenderPassRenderableUniforms, pass) {
            SetUniformState(uniform, renderable);
        }
    }

    bgfx::submit(viewId, bgfx::ProgramHandle {programHandle});
}

void RenderCommandList(Entity commandList, unsigned char viewId) {
    auto sceneRenderer = GetOwner(commandList);

    auto camera = GetSceneRendererCamera(sceneRenderer);
    auto scene = GetSceneRendererScene(sceneRenderer);
    auto renderTarget = GetSceneRendererTarget(sceneRenderer);

    if(!IsEntityValid(camera) || !IsEntityValid(scene) || !IsEntityValid(renderTarget)) return;

    auto viewport = GetSceneRendererViewport(sceneRenderer);
    auto pass = GetCommandListPass(commandList);
    auto renderState = GetRenderPassRenderState(pass);
    auto shaderCache = GetRenderPassShaderCache(pass);
    auto clearColor = GetRenderPassClearColor(pass);
    auto viewMat = GetFrustumViewMatrix(camera);
    auto projMat = GetFrustumProjectionMatrix(camera);

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

    SetShaderCacheProfile(shaderCache, shaderProfile);

    v2i renderTargetSize;
    if(IsEntityValid(renderTarget)) {
        renderTargetSize = GetRenderTargetSize(renderTarget);
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

    auto clearFlags = GetRenderPassClearTargets(pass);
    u16 bgfxClearFlags = 0;
    if(clearFlags & ClearTarget_Color) bgfxClearFlags |= BGFX_CLEAR_COLOR;
    if(clearFlags & ClearTarget_Depth) bgfxClearFlags |= BGFX_CLEAR_DEPTH;
    if(clearFlags & ClearTarget_Stencil) bgfxClearFlags |= BGFX_CLEAR_STENCIL;

    bgfx::setViewClear(
            viewId,
            bgfxClearFlags,
            RGBA2DWORD(clearColor.r, clearColor.g, clearColor.b, clearColor.a),
            GetRenderPassClearDepth(pass),
            GetRenderPassClearStencil(pass));

    bgfx::setViewRect(
            viewId,
            viewport.x * renderTargetSize.x,
            viewport.y * renderTargetSize.y,
            viewport.z * renderTargetSize.x,
            viewport.w * renderTargetSize.y
    );

    bgfx::setViewTransform(viewId, &viewMat, &projMat);

    bgfx::touch(viewId);


    {
        for_children(uniform, RenderPassSceneUniforms, pass) {
            SetUniformState(uniform, scene);
        }
    }

    {
        for_children(uniform, RenderPassCameraUniforms, pass) {
            SetUniformState(uniform, camera);
        }
    }

    {
        for_children(batch, CommandListBatches, commandList) {
            RenderBatch(viewId, batch, renderState, pass);
        }
    }
}

BeginUnit(BgfxCommandList)
    BeginComponent(BgfxCommandList)
    EndComponent()
EndUnit()