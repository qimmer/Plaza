//
// Created by Kim on 20-09-2018.
//

#include "Renderable.h"
#include "Material.h"
#include "Mesh.h"
#include "Uniform.h"

struct Renderable {
    Vector(RenderableUniformStates, Entity, 16)
    m4x4f RenderableWorldMatrix;
    Entity RenderableMaterial, RenderableSubMesh;
};

BeginUnit(Renderable)
    BeginComponent(Renderable)
        RegisterProperty(m4x4f, RenderableWorldMatrix)
        RegisterArrayProperty(UniformState, RenderableUniformStates)
        RegisterReferenceProperty(Material, RenderableMaterial)
        RegisterReferenceProperty(SubMesh, RenderableSubMesh)
    EndComponent()
EndUnit()