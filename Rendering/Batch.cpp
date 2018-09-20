//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Batch.h"
#include "Material.h"
#include "Mesh.h"
#include "ShaderCache.h"
#include "CommandList.h"
#include "Uniform.h"
#include "Renderable.h"

struct Batch {
    Entity BatchRenderable, BatchBinaryProgram;
    v4i BatchScissor;
    bool BatchDisabled;
};

LocalFunction(OnMaterialProgramChanged, void, Entity material, Entity oldProgram, Entity newProgram) {
    for_entity(batch, batchData, Batch) {
        if(batchData->BatchMaterial == material) {
            auto commandList = GetOwner(batch);
            auto shaderCache = GetCommandListShaderCache(commandList);

            SetBatchBinaryProgram(batch, GetShaderCacheBinaryProgram(shaderCache, newProgram));
        }
    }
}

LocalFunction(OnBatchRenderableChanged, void, Entity batch, Entity oldRenderable, Entity newRenderable) {
    auto commandList = GetOwner(batch);
    auto shaderCache = GetCommandListShaderCache(commandList);
    auto material = GetRenderableMaterial(newRenderable);

    SetBatchBinaryProgram(batch, GetShaderCacheBinaryProgram(shaderCache, GetMaterialProgram(material)));
}

LocalFunction(OnRenderableMaterialChanged, void, Entity renderable, Entity oldMaterial, Entity newMaterial) {
    for_entity(batch, data, Batch) {
        if(data->BatchRenderable == renderable) {
            OnBatchRenderableChanged(batch, renderable, renderable);
        }
    }
}

BeginUnit(Batch)
    BeginComponent(Batch)
        RegisterProperty(bool, BatchDisabled)
        RegisterProperty(v4i, BatchScissor)
        RegisterReferenceProperty(Renderable, BatchRenderable)
        RegisterReferencePropertyReadOnly(BinaryProgram, BatchBinaryProgram)
    EndComponent()

    RegisterSubscription(MaterialProgramChanged, OnMaterialProgramChanged, 0)
    RegisterSubscription(RenderableMaterialChanged, OnRenderableMaterialChanged, 0)
    RegisterSubscription(BatchRenderableChanged, OnBatchRenderableChanged, 0)
EndUnit()
