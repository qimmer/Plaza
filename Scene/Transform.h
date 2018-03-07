//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_TRANSFORM_H
#define PLAZA_TRANSFORM_H

#include <Core/Entity.h>

DeclareComponent(Transform)
DeclareService(TransformUpdateService)

DeclareComponentProperty(Transform, m4x4f, GlobalTransform)
DeclareComponentProperty(Transform, m4x4f, LocalTransform)

#endif //PLAZA_TRANSFORM_H
