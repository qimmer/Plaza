//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_TRANSFORM_H
#define PLAZA_TRANSFORM_H

#include <Core/Entity.h>

DeclareComponent(Transform)
DeclareService(TransformUpdateService)

DeclareComponentPropertyReactive(Transform, m4x4f, GlobalTransform)
DeclareComponentPropertyReactive(Transform, m4x4f, LocalTransform)

#endif //PLAZA_TRANSFORM_H
