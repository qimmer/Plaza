//
// Created by Kim on 15-08-2018.
//

#ifndef PLAZA_SHADERCACHE_H
#define PLAZA_SHADERCACHE_H

#include <Core/NativeUtils.h>

#define ShaderProfile_Unknown 0
#define ShaderProfile_HLSL_3_0 1
#define ShaderProfile_HLSL_4_0 2
#define ShaderProfile_HLSL_5_0 3
#define ShaderProfile_GLSL_2_1 4
#define ShaderProfile_GLSL_3_2 5
#define ShaderProfile_GLSL_4_1 6
#define ShaderProfile_GLSL_4_3 7
#define ShaderProfile_Metal_OSX 8
#define ShaderProfile_Metal_iOS 9

Unit(ShaderCache)
    Enum(ShaderProfile)

    Component(BinaryProgram)
        ReferenceProperty(Program, BinaryProgramProgram)
        ChildProperty(Stream, BinaryProgramVertexShader)
        ChildProperty(Stream, BinaryProgramPixelShader)

    Component(ShaderCache)
        Property(StringRef, ShaderCacheDefines)
        Property(u8, ShaderCacheProfile)
        Property(StringRef, ShaderCacheDirectoryPath)
        Property(StringRef, ShaderCacheIncludePath)
        ArrayProperty(BinaryProgram, ShaderCachePrograms)

    Event(ShaderCompile, Entity binaryProgram)

    Function(GetShaderCacheBinaryProgram, Entity, Entity shaderCache, Entity program)

#endif //PLAZA_SHADERCACHE_H
