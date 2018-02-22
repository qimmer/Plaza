//
// Created by Kim Johannsen on 29/01/2018.
//

#include "UniformState.h"


    struct UniformState {
        Entity UniformStateUniform;

        union {
            m4x4f UniformStateMat4;
            v4f UniformStateVec4;
            Entity UniformStateTexture;
        };

        u8 UniformStateStage;
    };

    DefineComponent(UniformState)
    EndComponent()

    DefineComponentProperty(UniformState, Entity, UniformStateUniform);
    DefineComponentProperty(UniformState, m4x4f, UniformStateMat4);
    DefineComponentProperty(UniformState, v4f, UniformStateVec4);
    DefineComponentProperty(UniformState, Entity, UniformStateTexture);
    DefineComponentProperty(UniformState, u8, UniformStateStage);
