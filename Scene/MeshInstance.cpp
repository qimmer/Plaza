//
// Created by Kim Johannsen on 28/01/2018.
//

#include "MeshInstance.h"
#include "SceneNode.h"
#include "Transform.h"

struct MeshInstance {
    Entity MeshInstanceMesh;
    Entity MeshInstanceMaterial;
};

DefineComponent(MeshInstance)
    Dependency(Transform)
    Dependency(SceneNode)
    DefineProperty(Entity, MeshInstanceMesh)
    DefineProperty(Entity, MeshInstanceMaterial)
EndComponent()

DefineComponentProperty(MeshInstance, Entity, MeshInstanceMesh)
DefineComponentProperty(MeshInstance, Entity, MeshInstanceMaterial)

