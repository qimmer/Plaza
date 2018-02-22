//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/Hierarchy.h>
#include <Foundation/Invalidation.h>
#include <Foundation/Stream.h>
#include "VertexBuffer.h"
#include "VertexAttribute.h"


    struct VertexDeclaration {

    };

    struct VertexBuffer {
        VertexBuffer() {
            memset(this, 0, sizeof(VertexBuffer));
        }
        Entity VertexBufferDeclaration, VertexBufferStream;
        bool VertexBufferDynamic;
    };

    DefineComponent(VertexDeclaration)
        Dependency(Invalidation)
        Dependency(Hierarchy)
    EndComponent()

    DefineComponent(VertexBuffer)
        Dependency(Invalidation)
    EndComponent()

    DefineService(VertexBuffer)
    EndService()

    DefineComponentProperty(VertexBuffer, Entity, VertexBufferDeclaration)
    DefineComponentProperty(VertexBuffer, bool, VertexBufferDynamic)
    DefineComponentProperty(VertexBuffer, Entity, VertexBufferStream)

    u32 GetVertexStride(Entity vertexDeclaration) {
        u32 stride = 0;
        for(auto attribute = GetFirstChild(vertexDeclaration); IsEntityValid(attribute); attribute = GetSibling(attribute)) {
            if(HasVertexAttribute(attribute)) {
                stride += GetTypeSize(GetVertexAttributeType(attribute));
            }
        }
        return stride;
    }

    static void OnInvalidated(Entity entity, bool before, bool after) {
        if(after) {
            if(HasVertexAttribute(entity)) {
                auto vertexDeclaration = GetParent(entity);
                if(IsEntityValid(vertexDeclaration) && HasVertexDeclaration(vertexDeclaration)) {
                    SetInvalidated(vertexDeclaration, true);
                }
            }

            if(HasVertexDeclaration(entity) || HasStream(entity)) {
                for(auto vertexBuffer = GetNextEntity(0); IsEntityValid(vertexBuffer); vertexBuffer = GetNextEntity(vertexBuffer)) {
                    if(!HasVertexBuffer(vertexBuffer)) {
                        continue;
                    }

                    if(GetVertexBufferDeclaration(vertexBuffer) == entity ||
                       GetVertexBufferStream(vertexBuffer) == entity) {
                        SetInvalidated(vertexBuffer, true);
                    }
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

