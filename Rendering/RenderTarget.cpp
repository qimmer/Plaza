//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Invalidation.h>
#include "RenderTarget.h"


    struct RenderTarget {
    };

    DefineComponent(RenderTarget)
        Dependency(Invalidation)
    EndComponent()

