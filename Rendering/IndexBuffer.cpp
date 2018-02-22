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
        Entity IndexBufferStream;
    };

    DefineComponent(IndexBuffer)
        Dependency(Invalidation)
    EndComponent()

    DefineService(IndexBuffer)
    EndService()

    DefineComponentProperty(IndexBuffer, bool, IndexBufferLong)
    DefineComponentProperty(IndexBuffer, bool, IndexBufferDynamic)
    DefineComponentProperty(IndexBuffer, Entity, IndexBufferStream)

    static void OnInvalidated(Entity entity, bool before, bool after) {
        if(after && HasStream(entity)) {
            for(auto indexBuffer = GetNextEntity(0); IsEntityValid(indexBuffer); indexBuffer = GetNextEntity(indexBuffer)) {
                if(!HasIndexBuffer(indexBuffer)) {
                    continue;
                }

                if(GetIndexBufferStream(indexBuffer) == entity) {
                    SetInvalidated(indexBuffer, true);
                }
            }
        }
    }

    static bool ServiceStart() {
        SubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }
