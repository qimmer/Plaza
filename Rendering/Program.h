//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_PROGRAM_H
#define PLAZA_PROGRAM_H

#include <Core/Entity.h>

DeclareComponent(Program)
DeclareComponentPropertyReactive(Program, Entity, ProgramBinaryVertexShader);
DeclareComponentPropertyReactive(Program, Entity, ProgramBinaryPixelShader);

Entity ResolveProgram(Entity vertexShader, Entity pixelShader, u8 profile, Entity shaderVariation);

DeclareService(Program)

#endif //PLAZA_PROGRAM_H
