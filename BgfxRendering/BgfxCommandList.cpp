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
#include <omp.h>

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

inline void RenderBatch(u32 viewId, bgfx::Encoder *encoder, Entity batch, Entity renderState, Entity pass) {
    auto batchData = GetBatchData(batch);
    auto renderStateData = GetRenderStateData(renderState);

    auto renderable = batchData->BatchRenderable;
    auto binaryProgram = batchData->BatchBinaryProgram;

    auto worldMatrix = GetTransformGlobalMatrix(renderable);

    auto subMesh = GetRenderableSubMesh(renderable);
    auto subMeshData = GetSubMeshData(subMesh);

    auto material = GetRenderableMaterial(renderable);

    auto mesh = GetOwner(subMesh);
    auto meshData = GetMeshData(mesh);

    auto vertexBuffer = meshData->MeshVertexBuffer; auto vertexBufferData = GetVertexBufferData(vertexBuffer);
    auto indexBuffer = meshData->MeshIndexBuffer; auto indexBufferData = GetIndexBufferData(indexBuffer);
    auto vertexDeclaration = vertexBufferData->VertexBufferDeclaration;

    auto programHandle = GetBgfxResourceHandle(binaryProgram);
    auto vertexBufferHandle = GetBgfxResourceHandle(vertexBuffer);
    auto indexBufferHandle = GetBgfxResourceHandle(indexBuffer);

    if(vertexBufferHandle == bgfx::kInvalidHandle) {
        Error(batch, "Invalid Vertex Buffer for renderable %s", GetUuid(renderable));
        return;
    }

    if(subMeshData->SubMeshNumIndices && indexBufferHandle == bgfx::kInvalidHandle) {
        Error(batch, "Invalid Index Buffer for renderable %s", GetUuid(renderable));
        return;
    }

    if(programHandle == bgfx::kInvalidHandle) {
        Error(batch, "Invalid Program for renderable %s", GetUuid(renderable));
        return;
    }

    encoder->setState(
            renderStateData->RenderStateWriteMask |
                    renderStateData->RenderStateBlendMode |
                    renderStateData->RenderStateDepthTest |
                    renderStateData->RenderStateMultisampleMode |
       subMeshData->SubMeshCullMode |
       subMeshData->SubMeshPrimitiveType
    );

    encoder->setTransform(&worldMatrix);

    if(vertexBufferData->VertexBufferDynamic) {
        encoder->setVertexBuffer(0, bgfx::DynamicVertexBufferHandle {vertexBufferHandle}, subMeshData->SubMeshStartVertex, subMeshData->SubMeshNumVertices);
    } else {
        encoder->setVertexBuffer(0, bgfx::VertexBufferHandle {vertexBufferHandle}, subMeshData->SubMeshStartVertex, subMeshData->SubMeshNumVertices);
    }

    if(indexBufferHandle != bgfx::kInvalidHandle) {
        if(indexBufferData->IndexBufferDynamic) {
            encoder->setIndexBuffer(bgfx::DynamicIndexBufferHandle {indexBufferHandle}, subMeshData->SubMeshStartIndex, subMeshData->SubMeshNumIndices);
        } else {
            encoder->setIndexBuffer(bgfx::IndexBufferHandle {indexBufferHandle}, subMeshData->SubMeshStartIndex, subMeshData->SubMeshNumIndices);
        }
    }

    {
        for_children(uniform, RenderPassMaterialUniforms, pass, {
            SetUniformState(uniform, material);
        });
    }

    {
        for_children(uniform, RenderPassRenderableUniforms, pass, {
            SetUniformState(uniform, renderable);
        });
    }

    encoder->submit(viewId, bgfx::ProgramHandle {programHandle});
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
    Validate(ComponentOf_BinaryProgram()); // Validate, as shaders could have changed after profile changed

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
        for_children(uniform, RenderPassSceneUniforms, pass, {
            SetUniformState(uniform, scene);
        });
    }

    {
        for_children(uniform, RenderPassCameraUniforms, pass, {
            SetUniformState(uniform, camera);
        });
    }

    {
        u32 numBatches = 0;
        auto batches = GetCommandListBatches(commandList, &numBatches);

        if(true) {
            #pragma omp parallel
            {
                auto threadnum = omp_get_thread_num();
                auto numthreads = omp_get_num_threads();
                auto low = numBatches*threadnum/numthreads;
                auto high = numBatches*(threadnum+1)/numthreads;

                auto encoder = bgfx::begin();

                for (auto i=low; i<high; i++) {
                    RenderBatch(viewId, encoder, batches[i], renderState, pass);
                }

                bgfx::end(encoder);
            }
        } else {
            auto encoder = bgfx::begin();

            for (auto i=0; i < numBatches; i++) {
                RenderBatch(viewId, encoder, batches[i], renderState, pass);
            }

            bgfx::end(encoder);
        }



    }
}

BeginUnit(BgfxCommandList)
    BeginComponent(BgfxCommandList)
    EndComponent()
EndUnit()