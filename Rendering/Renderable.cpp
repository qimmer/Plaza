//
// Created by Kim on 20-09-2018.
//

#include "Renderable.h"
#include "Material.h"
#include "Mesh.h"
#include "Uniform.h"

struct Renderable {
    Vector(RenderableUniformStates, Entity, 16)
    Entity RenderableMaterial, RenderableSubMesh;
};

BeginUnit(Renderable)
    BeginComponent(Renderable)
        RegisterBase(Transform)
        RegisterArrayProperty(UniformState, RenderableUniformStates)
        RegisterReferenceProperty(Material, RenderableMaterial)
        RegisterReferenceProperty(SubMesh, RenderableSubMesh)
    EndComponent()
EndUnit()