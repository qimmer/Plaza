//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/Hierarchy.h>
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

DefineComponent(VertexBuffer)
    Dependency(Stream)
    DefinePropertyReactive(Entity, VertexBufferDeclaration)
    DefinePropertyReactive(bool, VertexBufferDynamic)
EndComponent()

DefineComponentPropertyReactive(VertexBuffer, Entity, VertexBufferDeclaration)
DefineComponentPropertyReactive(VertexBuffer, bool, VertexBufferDynamic)




