//
// Created by Kim Johannsen on 13/01/2018.
//

#include <Core/String.h>
#include <Core/Hierarchy.h>
#include <Foundation/Stream.h>
#include <Foundation/MemoryStream.h>
#include "Shader.h"
#include "BinaryShader.h"
#include "ShaderCompiler.h"
#include "Program.h"

struct Shader {
    Entity ShaderDeclaration;
    u8 ShaderType;
};

DefineEnum(ShaderType, false)
    DefineFlag(ShaderType_Unknown)
    DefineFlag(ShaderType_Vertex)
    DefineFlag(ShaderType_Pixel)
    DefineFlag(ShaderType_Geometry)
    DefineFlag(ShaderType_Hull)
    DefineFlag(ShaderType_Domain)
    DefineFlag(ShaderType_Compute)
EndEnum()

DefineComponent(Shader)
    Dependency(Stream)
    Dependency(Hierarchy)

    DefineProperty(Entity, ShaderDeclaration)
    DefinePropertyEnum(u8, ShaderType, ShaderType)
EndComponent()

DefineComponentPropertyReactive(Shader, u8, ShaderType)
DefineComponentPropertyReactive(Shader, Entity, ShaderDeclaration)

