//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_BGFXSHADERCOMPILER_H
#define PLAZA_BGFXSHADERCOMPILER_H

#include <Core/NativeUtils.h>

#ifdef BGFXRENDERING_WITH_SHADER_COMPILER
    DeclareService(BgfxShaderCompiler)
#endif

void GetBinaryShaderFilePath(Entity binaryShader, char *out);

#endif //PLAZA_BGFXSHADERCOMPILER_H
