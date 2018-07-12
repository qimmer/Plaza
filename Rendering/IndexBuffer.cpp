//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Stream.h>
#include "IndexBuffer.h"

struct IndexBuffer {
    bool IndexBufferLong, IndexBufferDynamic;
};

BeginUnit(IndexBuffer)
    BeginComponent(IndexBuffer)
    RegisterBase(Stream)
EndComponent()

RegisterProperty(bool, IndexBufferLong)
RegisterProperty(bool, IndexBufferDynamic)
