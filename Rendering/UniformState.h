//
// Created by Kim Johannsen on 29/01/2018.
//

#ifndef PLAZA_UNIFORMSTATE_H
#define PLAZA_UNIFORMSTATE_H

#include <Core/NativeUtils.h>

Unit(UniformState)
    Component(UniformState)

        Property(Entity, UniformStateUniform);
DeclareComponentProperty(UniformState, m4x4f, UniformStateMat4);
DeclareComponentProperty(UniformState, v4f, UniformStateVec4);
DeclareComponentProperty(UniformState, Entity, UniformStateTexture);
DeclareComponentProperty(UniformState, u8, UniformStateStage);

void SetUniformStateState(Entity entity, u64 size, const void *data);
void GetUniformStateState(Entity entity, u64 size, void *data);
void* GetUniformStateStateRaw(Entity entity);

#endif //PLAZA_UNIFORMSTATE_H
