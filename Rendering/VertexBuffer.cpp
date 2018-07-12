//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/Node.h>
#include <Foundation/Stream.h>
#include "VertexBuffer.h"
#include "VertexAttribute.h"

struct VertexBuffer {
    VertexBuffer() {
        memset(this, 0, sizeof(VertexBuffer));
    }
    Entity VertexBufferDeclaration;
    bool VertexBufferDynamic;
};

BeginUnit(VertexBuffer)
    BeginComponent(VertexBuffer)
    RegisterBase(Stream)
    RegisterProperty(Entity, VertexBufferDeclaration)
    RegisterProperty(bool, VertexBufferDynamic)
EndComponent()

RegisterProperty(Entity, VertexBufferDeclaration)
RegisterProperty(bool, VertexBufferDynamic)




