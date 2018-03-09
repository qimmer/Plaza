//
// Created by Kim Johannsen on 09-03-2018.
//

#ifndef PLAZA_BGFXVERTEXDECLARATION_H
#define PLAZA_BGFXVERTEXDECLARATION_H

#include <Core/Service.h>
#include <Core/Entity.h>

DeclareComponent(BgfxVertexDeclaration)
DeclareService(BgfxVertexDeclaration)

void* GetBgfxVertexDeclarationHandle(Entity entity);

#endif //PLAZA_BGFXVERTEXDECLARATION_H
