//
// Created by Kim Johannsen on 29/01/2018.
//

#include "UniformState.h"

struct UniformState {
    Entity UniformStateUniform;

    union {
        m4x4f LargeArray[16];
        m4x4f UniformStateMat4;
        v4f UniformStateVec4;
        Entity UniformStateTexture;
    };

    u8 UniformStateStage;
};

BeginUnit(UniformState)
    BeginComponent(UniformState)
    RegisterProperty(Entity, UniformStateUniform)
    RegisterProperty(v4f, UniformStateVec4))
    RegisterProperty(Entity, UniformStateTexture))
    RegisterProperty(u8, UniformStateStage))
EndComponent()

RegisterProperty(Entity, UniformStateUniform);
RegisterProperty(m4x4f, UniformStateMat4);
RegisterProperty(v4f, UniformStateVec4);
RegisterProperty(Entity, UniformStateTexture);
RegisterProperty(u8, UniformStateStage)

API_EXPORT void SetUniformStateState(Entity entity, u64 size, const void *data) {
    Assert(size <= (sizeof(m4x4f)*16));
    memcpy(GetUniformStateData(entity)->LargeArray, data, size);
}

API_EXPORT void GetUniformStateState(Entity entity, u64 size, void *data) {
    Assert(size <= (sizeof(m4x4f)*16));
    memcpy(data, GetUniformStateData(entity)->LargeArray, size);
}

API_EXPORT void *GetUniformStateStateRaw(Entity entity) {
    return GetUniformStateData(entity)->LargeArray;
};
