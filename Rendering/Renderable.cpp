//
// Created by Kim on 20-09-2018.
//

#include "Renderable.h"
#include "Material.h"
#include "Mesh.h"
#include "Uniform.h"
#include <Scene/Transform.h>
#include <Scene/Scene.h>

BeginUnit(Renderable)
    BeginComponent(Renderable)
        RegisterBase(SceneNode)
        RegisterBase(Transform)
        RegisterReferenceProperty(Material, RenderableMaterial)
        RegisterReferenceProperty(SubMesh, RenderableSubMesh)
    EndComponent()
EndUnit()