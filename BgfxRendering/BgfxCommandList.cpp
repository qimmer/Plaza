//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/Algorithms.h>

#include <Foundation/Visibility.h>
#include <Foundation/AppLoop.h>

#include <Rendering/Uniform.h>
#include <Rendering/Mesh.h>
#include <Rendering/Material.h>
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
#include <Rendering/Texture.h>
#include <Core/Debug.h>

extern bgfx::UniformHandle SubTexture2DOffsetSizeUniform;

struct BgfxCommandList {
};

static void SetUniformState(Entity uniform, Entity entity, bgfx::Encoder *encoder) {
    auto uniformData = GetUniformData(uniform);

    auto component = GetOwner(uniformData->UniformEntityProperty);
    if(!HasComponent(entity, component)) return;

    auto value = GetPropertyValue(uniformData->UniformEntityProperty, entity);
    auto propertyType = GetPropertyType(uniformData->UniformEntityProperty);

    if(!propertyType) return;

    switch (propertyType) {
        case TypeOf_s8:
        case TypeOf_u8:
        case TypeOf_s16:
        case TypeOf_u16:
        case TypeOf_s32:
        case TypeOf_u32:
        case TypeOf_double:
        case TypeOf_float:
        case TypeOf_v2f:
        case TypeOf_v3f:
        case TypeOf_v4f:
        case TypeOf_v2i:
        case TypeOf_v3i:
        case TypeOf_v4i:
        case TypeOf_rgb8:
        case TypeOf_rgba8:
        case TypeOf_rgb32:
        case TypeOf_rgba32:
            value = Cast(value, TypeOf_v4f);
            break;
        case TypeOf_m3x3f:
        case TypeOf_m4x4f:
            value = Cast(value, TypeOf_m4x4f);
            break;
        case TypeOf_Entity:
            value = Cast(value, TypeOf_Entity);
            break;
        default:
            return;
            break;
    }

    auto uniformHandle = bgfx::UniformHandle{GetBgfxResourceHandle(uniform)};

    if(value.type != TypeOf_Entity) {
        encoder->setUniform(
                uniformHandle,
                &value.data,
                1);
    } else {
        auto texture = value.as_Entity;

        if(HasComponent(texture, ComponentOf_SubTexture2D())) {
            auto offset = GetSubTexture2DOffset(texture);
            auto size = GetSubTexture2DSize(texture);
            texture = GetOwner(texture); // Get actual texture (atlas)
            auto textureSize = GetTextureSize2D(texture);

            v4f value = {(float)offset.x / textureSize.x, (float)offset.y / textureSize.y, (float)size.x / textureSize.x, (float)size.y / textureSize.y};
            encoder->setUniform(SubTexture2DOffsetSizeUniform, &value);
        } else {
            v4f value = {0.0f, 0.0f, 1.0f, 1.0f};
            encoder->setUniform(SubTexture2DOffsetSizeUniform, &value);
        }

        if(!HasComponent(texture, ComponentOf_Texture())) {
            texture = TextureOf_White();
        }

        auto textureHandle = bgfx::TextureHandle{GetBgfxResourceHandle(texture)};
        encoder->setTexture(uniformData->UniformSamplerIndex, uniformHandle, textureHandle);
    }
}

inline void RenderBatch(u32 viewId, bgfx::Encoder *encoder, Entity batch, Entity renderState, Entity pass) {
    auto batchData = GetBatchData(batch);
    auto renderStateData = GetRenderStateData(renderState);
    if(!batchData) return;
    if(!renderStateData) return;

    auto renderable = batchData->BatchRenderable;
    auto binaryProgram = batchData->BatchBinaryProgram;

    auto transformData = GetTransformData(renderable);
    if(!transformData) return;
    auto worldMatrix = transformData->TransformGlobalMatrix;

    auto subMesh = GetRenderableSubMesh(renderable);
    auto subMeshData = GetSubMeshData(subMesh);

    if(!subMeshData) return;

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


    for_children(uniform, RenderPassMaterialUniforms, pass, {
        SetUniformState(uniform, material, encoder);
    });

    for_children(uniform, RenderPassMeshUniforms, pass, {
        SetUniformState(uniform, mesh, encoder);
    });

    for_children(uniform, RenderPassRenderableUniforms, pass, {
        SetUniformState(uniform, renderable, encoder);
    });

    encoder->submit(viewId, bgfx::ProgramHandle {programHandle}, transformData->TransformHierarchyLevel);
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
    bgfx::setViewMode(viewId, (bgfx::ViewMode::Enum)GetRenderPassSortMode(pass));

    bgfx::touch(viewId);

    auto primaryEncoder = bgfx::begin();
    for_children(uniform, RenderPassSceneUniforms, pass, {
        SetUniformState(uniform, scene, primaryEncoder);
    });

    for_children(uniform, RenderPassCameraUniforms, pass, {
        SetUniformState(uniform, camera, primaryEncoder);
    });

    u32 numBatches = 0;
    auto batches = GetCommandListBatches(commandList, &numBatches);

    if(numBatches > 0) {
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
            for (auto i=0; i < numBatches; i++) {
                RenderBatch(viewId, primaryEncoder, batches[i], renderState, pass);
            }
        }
    }

    bgfx::end(primaryEncoder);
}

LocalFunction(OnSubmitCommandLists, void, Entity appLoop) {
    auto i = 0;
    for_entity(commandList, data, CommandList, {
        RenderCommandList(commandList, i);
        ++i;
    });
}

BeginUnit(BgfxCommandList)
    BeginComponent(BgfxCommandList)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnSubmitCommandLists, AppLoopOf_BatchSubmission())
EndUnit()