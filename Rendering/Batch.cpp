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
