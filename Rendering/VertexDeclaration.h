//
// Created by Kim Johannsen on 09-03-2018.
//

#ifndef PLAZA_VERTEXDECLARATION_H
#define PLAZA_VERTEXDECLARATION_H

#include <Core/Entity.h>
#include <Core/Service.h>

DeclareService(VertexDeclaration)
DeclareComponent(VertexDeclaration)

u32 GetVertexStride(Entity vertexDeclaration);

#endif //PLAZA_VERTEXDECLARATION_H
