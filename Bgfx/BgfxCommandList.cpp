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
#include "BgfxCommandList.h"
#include "BgfxVertexBuffer.h"
#include "BgfxIndexBuffer.h"
#include "BgfxProgram.h"
#include "BgfxUniform.h"
#include "BgfxTexture2D.h"


    struct BgfxCommandList {
        u8 ViewId;
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

            if(IsTypeValid(uniformType)) {
                if(uniformType != TypeOf_Entity()) {
                    auto state = GetUniformStateMat4(uniformState);
                    bgfx::setUniform(
                            bgfx::UniformHandle{GetBgfxUniformHandle(uniform)},
                            &state,
                            GetUniformArrayCount(uniform));
                } else {
                    auto texture = GetUniformStateTexture(uniformState);
                    if(IsEntityValid(texture) && HasBgfxTexture2D(texture)) {
                        bgfx::setTexture(GetUniformStateStage(uniformState), bgfx::UniformHandle{GetBgfxUniformHandle(uniform)}, bgfx::TextureHandle{GetBgfxTexture2DHandle(texture)});
                    }
                }
            }
        }
    }

    void RenderBatch(u32 viewId, Entity entity) {
        auto scissor = GetBatchScissor(entity);
        auto transform = GetBatchWorldMatrix(entity);
        auto mesh = GetBatchMesh(entity);
        auto material = GetBatchMaterial(entity);

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

        bgfx::submit(viewId, bgfx::ProgramHandle {GetBgfxProgramHandle(program)});
    }

    void RenderCommandList(Entity entity) {
        auto viewId = GetBgfxCommandList(entity)->ViewId;
        auto viewport = GetCommandListViewport(entity);

        auto clearColor = GetCommandListClearColor(entity);
        auto viewMat = GetCommandListViewMatrix(entity);
        auto projMat = GetCommandListProjectionMatrix(entity);

        //bgfx::setViewFrameBuffer(viewId, )

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

            RenderBatch(viewId, batch);
        }
    }

    static bool ServiceStart() {

        return true;
    }

    static bool ServiceStop() {

        return true;
    }
