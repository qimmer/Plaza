//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Invalidation.h>
#include "RenderTarget.h"
#include "Context.h"


struct RenderTarget {
    };

    DefineComponent(RenderTarget)
        Dependency(Invalidation)
    EndComponent()

v2i GetRenderTargetSize(Entity entity) {
    Assert(IsEntityValid(entity));
    if(HasContext(entity)) {
        return GetContextSize(entity);
    }
    return {0, 0};
}

