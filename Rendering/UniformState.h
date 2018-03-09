//
// Created by Kim Johannsen on 29/01/2018.
//

#ifndef PLAZA_UNIFORMSTATE_H
#define PLAZA_UNIFORMSTATE_H

#include <Core/Entity.h>

DeclareComponent(UniformState)

DeclareComponentProperty(UniformState, Entity, UniformStateUniform);
DeclareComponentProperty(UniformState, m4x4f, UniformStateMat4);
DeclareComponentProperty(UniformState, v4f, UniformStateVec4);
DeclareComponentProperty(UniformState, Entity, UniformStateTexture);
DeclareComponentProperty(UniformState, u8, UniformStateStage);

#endif //PLAZA_UNIFORMSTATE_H
