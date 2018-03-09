//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_VERTEXBUFFER_H
#define PLAZA_VERTEXBUFFER_H

#include <Core/Entity.h>
#include <Core/Service.h>

DeclareComponent(VertexBuffer)

DeclareComponentPropertyReactive(VertexBuffer, Entity, VertexBufferDeclaration)
DeclareComponentPropertyReactive(VertexBuffer, bool, VertexBufferDynamic)

#endif //PLAZA_VERTEXBUFFER_H
