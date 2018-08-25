//
// Created by Kim Johannsen on 28/01/2018.
//

#include "MeshInstance.h"
#include "SceneNode.h"
#include "Transform.h"

BeginEnum(Layer, true)
    RegisterFlag(Layer_0)
    RegisterFlag(Layer_1)
    RegisterFlag(Layer_2)
    RegisterFlag(Layer_3)
    RegisterFlag(Layer_4)
    RegisterFlag(Layer_5)
    RegisterFlag(Layer_6)
    RegisterFlag(Layer_7)
EndEnum()

struct MeshInstance {
    Entity MeshInstanceMesh;
    Entity MeshInstanceMaterial;
    u8 MeshInstanceLayer;
};

BeginUnit(MeshInstance)
    BeginComponent(MeshInstance)
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

