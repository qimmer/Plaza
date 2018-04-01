//
// Created by Kim Johannsen on 29/01/2018.
//

#include "UniformState.h"

struct UniformState {
    Entity UniformStateUniform;

    union {
        m4x4f LargeArray[64];
        m4x4f UniformStateMat4;
        v4f UniformStateVec4;
        Entity UniformStateTexture;
    };

    u8 UniformStateStage;
};

DefineComponent(UniformState)
    DefinePropertyReactive(Entity, UniformStateUniform)
    DefineProperty(v4f, UniformStateVec4)
    DefineProperty(Entity, UniformStateTexture)
    DefineProperty(u8, UniformStateStage)
EndComponent()

DefineComponentPropertyReactive(UniformState, Entity, UniformStateUniform);
DefineComponentProperty(UniformState, m4x4f, UniformStateMat4);
DefineComponentProperty(UniformState, v4f, UniformStateVec4);
DefineComponentProperty(UniformState, Entity, UniformStateTexture);
DefineComponentProperty(UniformState, u8, UniformStateStage)

void SetUniformStateState(Entity entity, u64 size, const void *data) {
    Assert(size <= (sizeof(m4x4f)*64));
    memcpy(GetUniformState(entity)->LargeArray, data, size);
}

void GetUniformStateState(Entity entity, u64 size, void *data) {
    Assert(size <= (sizeof(m4x4f)*64));
    memcpy(data, GetUniformState(entity)->LargeArray, size);
};
