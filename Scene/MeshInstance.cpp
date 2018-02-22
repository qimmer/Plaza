//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Foundation/Invalidation.h>
#include "MeshInstance.h"
#include "SceneNode.h"
#include "Transform.h"


    struct MeshInstance {
        Entity MeshInstanceMesh;
        Entity MeshInstanceMaterial;
    };

    DefineComponent(MeshInstance)
        Dependency(Invalidation)
        Dependency(Transform)
        Dependency(SceneNode)
        DefineProperty(Entity, MeshInstanceMesh)
    EndComponent()

    DefineComponentProperty(MeshInstance, Entity, MeshInstanceMesh)
    DefineComponentProperty(MeshInstance, Entity, MeshInstanceMaterial)

