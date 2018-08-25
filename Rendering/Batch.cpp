//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Batch.h"
#include "Material.h"
#include "Mesh.h"
#include "ShaderCache.h"
#include "CommandList.h"
#include "Uniform.h"

struct Batch {
    Vector(BatchUniformStates, Entity, 8)
    Entity BatchMaterial, BatchSubMesh, BatchBinaryProgram;
    v4i BatchScissor;
    m4x4f BatchWorldMatrix;
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

LocalFunction(OnBatchMaterialChanged, void, Entity batch, Entity oldMaterial, Entity newMaterial) {
    auto commandList = GetOwner(batch);
    auto shaderCache = GetCommandListShaderCache(commandList);

    SetBatchBinaryProgram(batch, GetShaderCacheBinaryProgram(shaderCache, GetMaterialProgram(newMaterial)));
}

BeginUnit(Batch)
    BeginComponent(Batch)
        RegisterProperty(bool, BatchDisabled)
        RegisterProperty(v4i, BatchScissor)
        RegisterReferenceProperty(Material, BatchMaterial)
        RegisterReferenceProperty(SubMesh, BatchSubMesh)
        RegisterReferenceProperty(BinaryProgram, BatchBinaryProgram)
        RegisterProperty(m4x4f, BatchWorldMatrix)
        RegisterArrayProperty(UniformState, BatchUniformStates)
    EndComponent()

    RegisterSubscription(MaterialProgramChanged, OnMaterialProgramChanged, 0)
    RegisterSubscription(BatchMaterialChanged, OnBatchMaterialChanged, 0)
EndUnit()
