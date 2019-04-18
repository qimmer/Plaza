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
    auto uniformData = GetUniform(uniform);

    auto component = GetOwnership(uniformData.UniformEntityProperty).Owner;
    auto propertyType = GetProperty(uniformData.UniformEntityProperty).PropertyType;

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
        value = GetPropertyValue(uniformData.UniformEntityProperty, entity);
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

    auto uniformHandle = bgfx::UniformHandle{GetBgfxResource(uniform).BgfxResourceHandle};

    if(value.type != TypeOf_Entity) {
        encoder->setUniform(
                uniformHandle,
                &value.data,
                1);
    } else {
        auto texture = value.as_Entity;

        if(HasComponent(texture, ComponentOf_SubTexture2D())) {
            auto subTexture2D = GetSubTexture2D(texture);
            auto offset = subTexture2D.SubTexture2DOffset;
            auto size = subTexture2D.SubTexture2DSize;

            texture = GetOwnership(texture).Owner; // Get actual texture (atlas)
            auto textureSize = GetTexture2D(texture).TextureSize2D;

            uvOffsetSizePerSampler[uniformData.UniformSamplerIndex] = {(float)offset.x / textureSize.x, (float)offset.y / textureSize.y, (float)size.x / textureSize.x, (float)size.y / textureSize.y};
        } else {
            uvOffsetSizePerSampler[uniformData.UniformSamplerIndex] = {0.0f, 0.0f, 1.0f, 1.0f};
        }

        if(!HasComponent(texture, ComponentOf_Texture())) {
            Warning(entity, "%s is missing texture value for uniform %s.", GetIdentification(entity).Uuid, GetIdentification(uniform).Uuid);
            texture = whiteTexture;
        }

        auto textureHandle = bgfx::TextureHandle{GetBgfxResource(texture).BgfxResourceHandle};
        encoder->setTexture(uniformData.UniformSamplerIndex, uniformHandle, textureHandle);
    }
}

inline void RenderBatch(u32 viewId, bgfx::Encoder *encoder, Entity batch, Entity renderState, const RenderPass& passData, v4f *uvOffsetSizePerSampler) {
    auto batchData = GetBatch(batch);
    auto renderStateData = GetRenderState(renderState);
    
    auto renderable = batchData.BatchRenderable;
    auto binaryProgram = batchData.BatchBinaryProgram;

    auto transformData = GetWorldTransform(renderable);

    if(GetVisibility(renderable).HierarchiallyHidden) return;

    auto renderableData = GetRenderable(renderable);

    // Support setting submesh to a mesh and then use first submesh, just for convenience
    if(HasComponent(renderableData.RenderableSubMesh, ComponentOf_Mesh())) {
        auto meshData = GetMesh(renderableData.RenderableSubMesh);

        if(!meshData.MeshSubMeshes.GetSize()) return;

        renderableData.RenderableSubMesh = meshData.MeshSubMeshes[0];
    }

    auto subMeshData = GetSubMesh(renderableData.RenderableSubMesh);

    auto mesh = GetOwnership(renderableData.RenderableSubMesh).Owner;
    auto meshData = GetMesh(mesh);

    auto vertexBuffer = meshData.MeshVertexBuffer;
    auto vertexBufferData = GetVertexBuffer(vertexBuffer);

    auto indexBuffer = meshData.MeshIndexBuffer;
    auto indexBufferData = GetIndexBuffer(indexBuffer);

    auto vertexDeclaration = vertexBufferData.VertexBufferDeclaration;

    auto programHandle = GetBgfxResource(binaryProgram).BgfxResourceHandle;
    auto vertexBufferHandle = GetBgfxResource(vertexBuffer).BgfxResourceHandle;
    auto indexBufferHandle = GetBgfxResource(indexBuffer).BgfxResourceHandle;

    if(vertexBufferHandle == bgfx::kInvalidHandle) {
        //Error(batch, "Invalid Vertex Buffer for renderable %s", GetIdentification(renderable).Uuid);
        return;
    }

    if(subMeshData.SubMeshNumIndices && indexBufferHandle == bgfx::kInvalidHandle) {
        //Error(batch, "Invalid Index Buffer for renderable %s", GetIdentification(renderable).Uuid);
        return;
    }

    if(programHandle == bgfx::kInvalidHandle) {
        Error(batch, "Invalid Program for renderable %s", GetIdentification(renderable).Uuid);
        return;
    }

    encoder->setState(
        renderStateData.RenderStateWriteMask |
        renderStateData.RenderStateBlendMode |
        renderStateData.RenderStateDepthTest |
        renderStateData.RenderStateMultisampleMode |
        subMeshData.SubMeshCullMode |
        subMeshData.SubMeshPrimitiveType
    );

    if(renderableData.RenderableScissor.z != 0 || renderableData.RenderableScissor.w != 0) {
        encoder->setScissor(renderableData.RenderableScissor.x, renderableData.RenderableScissor.y, renderableData.RenderableScissor.z, renderableData.RenderableScissor.w);
    }

    encoder->setTransform(&transformData.WorldTransformMatrix[0].x);

    if(vertexBufferData.VertexBufferDynamic) {
        encoder->setVertexBuffer(0, bgfx::DynamicVertexBufferHandle {vertexBufferHandle}, subMeshData.SubMeshStartVertex, subMeshData.SubMeshNumVertices);
    } else {
        encoder->setVertexBuffer(0, bgfx::VertexBufferHandle {vertexBufferHandle}, subMeshData.SubMeshStartVertex, subMeshData.SubMeshNumVertices);
    }

    if(indexBufferHandle != bgfx::kInvalidHandle) {
        if(indexBufferData.IndexBufferDynamic) {
            encoder->setIndexBuffer(bgfx::DynamicIndexBufferHandle {indexBufferHandle}, subMeshData.SubMeshStartIndex, subMeshData.SubMeshNumIndices);
        } else {
            encoder->setIndexBuffer(bgfx::IndexBufferHandle {indexBufferHandle}, subMeshData.SubMeshStartIndex, subMeshData.SubMeshNumIndices);
        }
    }

    for(auto uniform1 : passData.RenderPassMaterialUniforms) {
        SetUniformState(uniform1, renderableData.RenderableMaterial, encoder, uvOffsetSizePerSampler);
    }

    for(auto uniform2 : passData.RenderPassMeshUniforms) {
        SetUniformState(uniform2, mesh, encoder, uvOffsetSizePerSampler);
    }

    for(auto uniform3 : passData.RenderPassRenderableUniforms) {
        SetUniformState(uniform3, renderable, encoder, uvOffsetSizePerSampler);
    }

    encoder->setUniform(u_uvOffsetSizePerSampler, uvOffsetSizePerSampler, 8);

    encoder->submit(viewId, bgfx::ProgramHandle {programHandle}, transformData.WorldTransformMatrix->z);
}

void RenderCommandList(Entity commandList, unsigned char viewId) {
    auto sceneRenderer = GetOwnership(commandList).Owner;
    auto sceneRendererData = GetSceneRenderer(sceneRenderer);
    auto commandListData = GetCommandList(commandList);

    auto camera = sceneRendererData.SceneRendererCamera;
    auto scene = sceneRendererData.SceneRendererScene;
    auto renderTarget = sceneRendererData.SceneRendererTarget;

    if(!IsEntityValid(camera) || !IsEntityValid(scene) || !IsEntityValid(renderTarget)) return;

    if(GetVisibility(camera).HierarchiallyHidden) return;

    auto viewport = sceneRendererData.SceneRendererViewport;
    auto pass = commandListData.CommandListPass;

    auto passData = GetRenderPass(pass);
    auto renderState = passData.RenderPassRenderState;
    auto shaderCache = passData.RenderPassShaderCache;
    auto clearColor = passData.RenderPassClearColor;
	auto frustumData = GetFrustum(camera);

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

    auto shaderCacheData = GetShaderCache(shaderCache);
    if(shaderCacheData.ShaderCacheProfile != shaderProfile) {
        shaderCacheData.ShaderCacheProfile = shaderProfile;
        SetShaderCache(shaderCache, shaderCacheData);
    }

    v2i renderTargetSize;
    if(IsEntityValid(renderTarget)) {
        renderTargetSize = GetRenderTarget(renderTarget).RenderTargetSize;
        if(HasComponent(renderTarget, ComponentOf_BgfxRenderContext())) {
            bgfx::FrameBufferHandle fb = {GetBgfxResource(renderTarget).BgfxResourceHandle};
            if(fb.idx != bgfx::kInvalidHandle) {
                bgfx::setViewFrameBuffer(viewId, fb);
            }

        }

        if(HasComponent(renderTarget, ComponentOf_BgfxOffscreenRenderTarget())) {
            bgfx::FrameBufferHandle fb = {GetBgfxResource(renderTarget).BgfxResourceHandle};
            if(fb.idx != bgfx::kInvalidHandle) {
                bgfx::setViewFrameBuffer(viewId, fb);
            }
        }
    }

    auto clearFlags = passData.RenderPassClearTargets;
    u16 bgfxClearFlags = 0;
    if(clearFlags & ClearTarget_Color) bgfxClearFlags |= BGFX_CLEAR_COLOR;
    if(clearFlags & ClearTarget_Depth) bgfxClearFlags |= BGFX_CLEAR_DEPTH;
    if(clearFlags & ClearTarget_Stencil) bgfxClearFlags |= BGFX_CLEAR_STENCIL;

    bgfx::setViewClear(
            viewId,
            bgfxClearFlags,
            RGBA2DWORD(clearColor.r, clearColor.g, clearColor.b, clearColor.a),
            passData.RenderPassClearDepth,
            passData.RenderPassClearStencil);

    bgfx::setViewRect(
            viewId,
            viewport.x * renderTargetSize.x,
            viewport.y * renderTargetSize.y,
            viewport.z * renderTargetSize.x,
            viewport.w * renderTargetSize.y
    );

    bgfx::setViewTransform(viewId, &frustumData.FrustumViewMatrix[0].x, &frustumData.FrustumProjectionMatrix[0].x);
    bgfx::setViewMode(viewId, (bgfx::ViewMode::Enum)passData.RenderPassSortMode);

    bgfx::touch(viewId);

    auto primaryEncoder = bgfx::begin();

    v4f uvOffsetSizePerSampler[8];

    for(auto uniform1 : passData.RenderPassSceneUniforms) {
        SetUniformState(uniform1, scene, primaryEncoder, uvOffsetSizePerSampler);
    }

    for(auto uniform2 : passData.RenderPassCameraUniforms) {
        SetUniformState(uniform2, camera, primaryEncoder, uvOffsetSizePerSampler);
    }

    if(false) {
        #pragma omp parallel
        {
            auto threadnum = omp_get_thread_num();
            auto numthreads = omp_get_num_threads();
            auto low = commandListData.CommandListBatches.GetSize()*threadnum/numthreads;
            auto high = commandListData.CommandListBatches.GetSize()*(threadnum+1)/numthreads;

            auto encoder = bgfx::begin();

            for (auto i=low; i<high; i++) {
                RenderBatch(viewId, encoder, commandListData.CommandListBatches[i], renderState, passData, uvOffsetSizePerSampler);
            }

            bgfx::end(encoder);
        }
    } else {
        for (auto batch : commandListData.CommandListBatches) {
            RenderBatch(viewId, primaryEncoder, batch, renderState, passData, uvOffsetSizePerSampler);
        }
    }
    

    bgfx::end(primaryEncoder);
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    auto i = 0;
    for_entity(commandList, ComponentOf_CommandList()) {
        RenderCommandList(commandList, i);
        ++i;
    }
}

BeginUnit(BgfxCommandList)
    BeginComponent(BgfxCommandList)
    EndComponent()

    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_BatchSubmission)
EndUnit()