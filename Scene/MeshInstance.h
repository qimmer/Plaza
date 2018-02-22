//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_MESHINSTANCE_H
#define PLAZA_MESHINSTANCE_H

#include <Core/Entity.h>


    DeclareComponent(MeshInstance)

    DeclareComponentProperty(MeshInstance, Entity, MeshInstanceMesh)
    DeclareComponentProperty(MeshInstance, Entity, MeshInstanceMaterial)

#endif //PLAZA_MESHINSTANCE_H
