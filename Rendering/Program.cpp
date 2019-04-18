//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Stream.h>
#include "Program.h"

Prefab(ShaderSource)

BeginUnit(Program)
    BeginPrefab(ShaderSource)
    EndPrefab()
    BeginComponent(Program)
        BeginChildProperty(ProgramVertexShaderSource)
        EndChildProperty()
        BeginChildProperty(ProgramPixelShaderSource)
        EndChildProperty()
        BeginChildProperty(ProgramDeclShaderSource)
        EndChildProperty()
    EndComponent()
EndUnit()
