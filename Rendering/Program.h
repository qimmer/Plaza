//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_PROGRAM_H
#define PLAZA_PROGRAM_H

#include <Core/NativeUtils.h>

Unit(Program)
    Component(Program)
        ChildProperty(Stream, ProgramVertexShaderSource)
        ChildProperty(Stream, ProgramPixelShaderSource)
        ChildProperty(Stream, ProgramDeclShaderSource)

#endif //PLAZA_PROGRAM_H
