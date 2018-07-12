//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_PROGRAM_H
#define PLAZA_PROGRAM_H

#include <Core/NativeUtils.h>

Unit(Program)
    Component(Program)
        Property(Entity, ProgramBinaryVertexShader);
        Property(Entity, ProgramBinaryPixelShader);

Entity ResolveProgram(Entity vertexShader, Entity pixelShader, u8 profile, Entity shaderVariation);


#endif //PLAZA_PROGRAM_H
