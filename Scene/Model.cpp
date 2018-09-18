//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Model.h"
#include "SceneNode.h"
#include "Transform.h"

struct Model {
    Entity ModelSubMesh;
    Entity ModelMaterial;
};

BeginUnit(Model)
    BeginComponent(Model)
        RegisterBase(Transform)
        RegisterBase(SceneNode)
    RegisterProperty(Entity, MeshInstanceMesh)
    RegisterProperty(Entity, MeshInstanceMaterial)
        RegisterPropertyEnum(u8, MeshInstanceLayer, Layer)
EndComponent()
EndUnit()
(Entity, MeshInstanceMesh)
RegisterProperty(Entity, MeshInstanceMaterial)
RegisterProperty(u8, MeshInstanceLayer)

