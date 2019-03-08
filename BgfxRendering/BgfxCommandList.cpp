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
#include <omp.h>
#include <Rendering/Texture.h>
#include <Core/Debug.h>

extern bgfx::UniformHandle u_uvOffsetSizePerSampler;

struct BgfxCommandList {
};

static void SetUniformState(Entity uniform, Entity entity, bgfx::Encoder *encoder, v4f *uvOffsetSizePerSampler) {
    auto uniformData = GetUniformData(uniform);

    auto component = GetOwner(uniformData->UniformEntityProperty);
    auto propertyType = GetPropertyType(uniformData->UniformEntityProperty);

    if(!propertyType) return;

    Variant value;
    auto whiteTexture = TextureOf_White();

    if(!HasComponent(entity, component)) {
        // Make default value

        value = Variant_Default;
        value.type = propertyType;

        if(propertyType == TypeOf_Entity) {
            value.as_Entity = whiteTexture;
        }
    } else {
        value = GetPropertyValue(uniformData->UniformEntityProperty, entity);
    }

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

            uvOffsetSizePerSampler[uniformData->UniformSamplerIndex] = {(float)offset.x / textureSize.x, (float)offset.y / textureSize.y, (float)size.x / textureSize.x, (float)size.y / textureSize.y};
        } else {
            uvOffsetSizePerSampler[uniformData->UniformSamplerIndex] = {0.0f, 0.0f, 1.0f, 1.0f};
        }

        if(!HasComponent(texture, ComponentOf_Texture())) {
            Warning(entity, "%s is missing texture value for uniform %s.", GetUuid(entity), GetUuid(uniform));
            texture = whiteTexture;
        }

        auto textureHandle = bgfx::TextureHandle{GetBgfxResourceHandle(texture)};
        encoder->setTexture(uniformData->UniformSamplerIndex, uniformHandle, textureHandle);
    }
}

inline void RenderBatch(u32 viewId, bgfx::Encoder *encoder, Entity batch, Entity renderState, Entity pass, v4f *uvOffsetSizePerSampler) {
    auto batchData = GetBatchData(batch);
    auto renderStateData = GetRenderStateData(renderState);
    if(!batchData) return;
    if(!renderStateData) return;

    auto renderable = batchData->BatchRenderable;
    auto binaryProgram = batchData->BatchBinaryProgram;

    auto transformData = GetTransformData(renderable);
    if(!transformData) return;

    if(GetHierarchiallyHidden(renderable)) return;

    auto subMesh = GetRenderableSubMesh(renderable);

    // Support setting submesh to a mesh and then use first submesh, just for convenience
    if(HasComponent(subMesh, ComponentOf_Mesh())) {
        auto& subMeshes = GetMeshSubMeshes(subMesh);
        if(subMeshes.size() < 1) return;

        subMesh = subMeshes[0];
    }

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
        //Error(batch, "Invalid Vertex Buffer for renderable %s", GetUuid(renderable));
        return;
    }

    if(subMeshData->SubMeshNumIndices && indexBufferHandle == bgfx::kInvalidHandle) {
        //Error(batch, "Invalid Index Buffer for renderable %s", GetUuid(renderable));
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

    encoder->setTransform(&transformData->TransformGlobalMatrix[0].x);

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

    for_children(uniform1, RenderPassMaterialUniforms, pass) {
        SetUniformState(uniform1, material, encoder, uvOffsetSizePerSampler);
    }

    for_children(uniform2, RenderPassMeshUniforms, pass) {
        SetUniformState(uniform2, mesh, encoder, uvOffsetSizePerSampler);
    }

    for_children(uniform3, RenderPassRenderableUniforms, pass) {
        SetUniformState(uniform3, renderable, encoder, uvOffsetSizePerSampler);
    }

    encoder->setUniform(u_uvOffsetSizePerSampler, uvOffsetSizePerSampler, 8);

    encoder->submit(viewId, bgfx::ProgramHandle {programHandle}, transformData->TransformHierarchyLevel);
}

void RenderCommandList(Entity commandList, unsigned char viewId) {
    auto sceneRenderer = GetOwner(commandList);

    auto camera = GetSceneRendererCamera(sceneRenderer);
    auto scene = GetSceneRendererScene(sceneRenderer);
    auto renderTarget = GetSceneRendererTarget(sceneRenderer);

    if(!IsEntityValid(camera) || !IsEntityValid(scene) || !IsEntityValid(renderTarget)) return;

    if(GetHierarchiallyHidden(camera)) return;

    auto viewport = GetSceneRendererViewport(sceneRenderer);
    auto pass = GetCommandListPass(commandList);
    auto renderState = GetRenderPassRenderState(pass);
    auto shaderCache = GetRenderPassShaderCache(pass);
    auto clearColor = GetRenderPassClearColor(pass);
	auto frustumData = GetFrustumData(camera);

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

    bgfx::setViewTransform(viewId, &frustumData->FrustumViewMatrix[0].x, &frustumData->FrustumProjectionMatrix[0].x);
    bgfx::setViewMode(viewId, (bgfx::ViewMode::Enum)GetRenderPassSortMode(pass));

    bgfx::touch(viewId);

    auto primaryEncoder = bgfx::begin();

    v4f uvOffsetSizePerSampler[8];

    for_children(uniform1, RenderPassSceneUniforms, pass) {
        SetUniformState(uniform1, scene, primaryEncoder, uvOffsetSizePerSampler);
    }

    for_children(uniform2, RenderPassCameraUniforms, pass) {
        SetUniformState(uniform2, camera, primaryEncoder, uvOffsetSizePerSampler);
    }

    auto& batches = GetCommandListBatches(commandList);

        if(false) {
            #pragma omp parallel
            {
                auto threadnum = omp_get_thread_num();
                auto numthreads = omp_get_num_threads();
                auto low = batches.size()*threadnum/numthreads;
                auto high = batches.size()*(threadnum+1)/numthreads;

                auto encoder = bgfx::begin();

                for (auto i=low; i<high; i++) {
                    RenderBatch(viewId, encoder, batches[i], renderState, pass, uvOffsetSizePerSampler);
                }

                bgfx::end(encoder);
            }
        } else {
            for (auto i=0; i < batches.size(); i++) {
                RenderBatch(viewId, primaryEncoder, batches[i], renderState, pass, uvOffsetSizePerSampler);
            }
        }
    

    bgfx::end(primaryEncoder);
}

LocalFunction(OnSubmitCommandLists, void, Entity appLoop) {
    auto i = 0;
    for_entity(commandList, data, CommandList) {
        RenderCommandList(commandList, i);
        ++i;
    }
}

BeginUnit(BgfxCommandList)
    BeginComponent(BgfxCommandList)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnSubmitCommandLists, AppLoopOf_BatchSubmission())
EndUnit()