//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_PROGRAM_H
#define PLAZA_PROGRAM_H

#include <Core/Entity.h>

DeclareComponent(Program)
DeclareComponentPropertyReactive(Program, Entity, VertexShader);
DeclareComponentPropertyReactive(Program, Entity, PixelShader);
DeclareService(Program)

#endif //PLAZA_PROGRAM_H
