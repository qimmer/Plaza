//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_MESHINSTANCE_H
#define PLAZA_MESHINSTANCE_H

#include <Core/Entity.h>

#define Layer_0 (1 << 0)
#define Layer_1 (1 << 1)
#define Layer_2 (1 << 2)
#define Layer_3 (1 << 3)
#define Layer_4 (1 << 4)
#define Layer_5 (1 << 5)
#define Layer_6 (1 << 6)
#define Layer_7 (1 << 7)

DeclareEnum(Layer)

DeclareComponent(MeshInstance)

DeclareComponentPropertyReactive(MeshInstance, Entity, MeshInstanceMesh)
DeclareComponentPropertyReactive(MeshInstance, Entity, MeshInstanceMaterial)
DeclareComponentPropertyReactive(MeshInstance, u8, MeshInstanceLayer)

#endif //PLAZA_MESHINSTANCE_H
