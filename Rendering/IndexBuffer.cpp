//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Invalidation.h>
#include <Foundation/Stream.h>
#include "IndexBuffer.h"


    struct IndexBuffer {
        IndexBuffer() {
            memset(this, 0, sizeof(IndexBuffer));
        }

        bool IndexBufferLong, IndexBufferDynamic;
    };

    DefineComponent(IndexBuffer)
        Dependency(Invalidation)
        Dependency(Stream)
    EndComponent()

    DefineService(IndexBuffer)
    EndService()

    DefineComponentProperty(IndexBuffer, bool, IndexBufferLong)
    DefineComponentProperty(IndexBuffer, bool, IndexBufferDynamic)

    static bool ServiceStart() {
        return true;
    }

    static bool ServiceStop() {
        return true;
    }
