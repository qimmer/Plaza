//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_VERTEXBUFFER_H
#define PLAZA_VERTEXBUFFER_H

#include <Core/Entity.h>
#include <Core/Service.h>


    DeclareService(VertexBuffer)

    DeclareComponent(VertexBuffer)
    DeclareComponent(VertexDeclaration)

    DeclareComponentProperty(VertexBuffer, Entity, VertexBufferDeclaration)
    DeclareComponentProperty(VertexBuffer, bool, VertexBufferDynamic)

    u32 GetVertexStride(Entity vertexDeclaration);


#endif //PLAZA_VERTEXBUFFER_H
