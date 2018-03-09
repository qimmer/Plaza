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

struct Shader {
    Entity ShaderProgram;
    u8 ShaderType;
};

DefineComponent(Shader)
    Dependency(Stream)
    Dependency(Hierarchy)
EndComponent()

DefineComponentPropertyReactive(Shader, u8, ShaderType)

DefineService(Shader)
EndService()

Entity GetBinaryShader(Entity shader, u8 profile) {
    for(auto binaryShader = GetFirstChild(shader); binaryShader; binaryShader = GetSibling(binaryShader)) {
        if(!HasBinaryShader(binaryShader)) continue;
        if(GetBinaryShaderProfile(binaryShader) == profile) {
            return binaryShader;
        }
    }

    char binaryShaderPath[PATH_MAX];
    sprintf(binaryShaderPath, "%s/%d", GetEntityPath(shader), (int)profile);

    auto binaryShader = CreateBinaryShader(binaryShaderPath);
    SetSourceShader(binaryShader, shader);
    SetBinaryShaderProfile(binaryShader, profile);

    return binaryShader;
}

static bool ServiceStart() {
    return true;
}

static bool ServiceStop() {
    return true;
}
