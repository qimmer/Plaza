//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Stream.h>
#include "IndexBuffer.h"

struct IndexBuffer {
    bool IndexBufferLong, IndexBufferDynamic;
};

DefineComponent(IndexBuffer)
    Dependency(Stream)
EndComponent()

DefineComponentPropertyReactive(IndexBuffer, bool, IndexBufferLong)
DefineComponentPropertyReactive(IndexBuffer, bool, IndexBufferDynamic)
