//
// Created by Kim Johannsen on 14/01/2018.
//

#include "RenderTarget.h"
#include "Context.h"

struct RenderTarget {
};

DefineComponent(RenderTarget)
EndComponent()

v2i GetRenderTargetSize(Entity entity) {
    Assert(IsEntityValid(entity));
    if(HasContext(entity)) {
        return GetContextSize(entity);
    }
    return {0, 0};
}

