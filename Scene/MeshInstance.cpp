//
// Created by Kim Johannsen on 28/01/2018.
//

#include "MeshInstance.h"
#include "SceneNode.h"
#include "Transform.h"

DefineEnum(Layer, true)
    DefineFlag(Layer_0)
    DefineFlag(Layer_1)
    DefineFlag(Layer_2)
    DefineFlag(Layer_3)
    DefineFlag(Layer_4)
    DefineFlag(Layer_5)
    DefineFlag(Layer_6)
    DefineFlag(Layer_7)
EndEnum()

struct MeshInstance {
    Entity MeshInstanceMesh;
    Entity MeshInstanceMaterial;
    u8 MeshInstanceLayer;
};

DefineComponent(MeshInstance)
    Dependency(Transform)
    Dependency(SceneNode)
    DefinePropertyReactive(Entity, MeshInstanceMesh)
    DefinePropertyReactive(Entity, MeshInstanceMaterial)
    DefinePropertyReactiveEnum(u8, MeshInstanceLayer, Layer)
EndComponent()

DefineComponentPropertyReactive(MeshInstance, Entity, MeshInstanceMesh)
DefineComponentPropertyReactive(MeshInstance, Entity, MeshInstanceMaterial)
DefineComponentPropertyReactive(MeshInstance, u8, MeshInstanceLayer)

