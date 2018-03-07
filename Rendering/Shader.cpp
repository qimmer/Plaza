//
// Created by Kim Johannsen on 13/01/2018.
//

#include <Core/String.h>
#include <Foundation/Invalidation.h>
#include <Core/Hierarchy.h>
#include <Foundation/Stream.h>
#include <Foundation/MemoryStream.h>
#include "Shader.h"
#include "BinaryShader.h"


    struct Shader {
        Shader() {
            memset(this, 0, sizeof(Shader));
        }
        Entity ShaderProgram;
        u8 ShaderType;
    };

    DefineComponent(Shader)
        Dependency(Invalidation)
        Dependency(Stream)
        Dependency(Hierarchy)
    EndComponent()

    DefineComponentProperty(Shader, u8, ShaderType)

    DefineService(Shader)
    EndService()

    Entity GetBinaryShader(Entity shader, StringRef profile) {
        for(auto binaryShader = GetFirstChild(shader); IsEntityValid(binaryShader); binaryShader = GetSibling(binaryShader)) {
            if(!HasBinaryShader(binaryShader)) continue;
            if(strcmp(GetBinaryShaderProfile(binaryShader), profile) == 0) {
                return binaryShader;
            }
        }

        char binaryShaderPath[PATH_MAX];
        sprintf(binaryShaderPath, "%s/%s", GetEntityPath(shader), profile);

        auto binaryShader = CreateBinaryShader(binaryShaderPath);
        SetSourceShader(binaryShader, shader);
        SetBinaryShaderProfile(binaryShader, profile);

        return binaryShader;
    }

    static void OnInvalidated(Entity entity, bool before, bool after) {

    }

    static bool ServiceStart() {
        SubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }
