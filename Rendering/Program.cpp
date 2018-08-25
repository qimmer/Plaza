//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Stream.h>
#include "Program.h"

struct Program {
    Entity ProgramVertexShaderSource, ProgramPixelShaderSource, ProgramDeclShaderSource;
};

BeginUnit(Program)
    BeginComponent(Program)
        RegisterChildProperty(Stream, ProgramVertexShaderSource)
        RegisterChildProperty(Stream, ProgramPixelShaderSource)
        RegisterChildProperty(Stream, ProgramDeclShaderSource)
    EndComponent()
EndUnit()
