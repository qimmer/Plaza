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
        Entity ShaderSourceStream, ShaderVaryingDefStream;
        u8 ShaderType;
    };

    DefineComponent(Shader)
        Dependency(Invalidation)
        DefineProperty(Entity, ShaderSourceStream)
        DefineProperty(Entity, ShaderVaryingDefStream)
    EndComponent()

    DefineComponentProperty(Shader, Entity, ShaderSourceStream)
    DefineComponentProperty(Shader, Entity, ShaderVaryingDefStream)
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
        char binaryShaderStreamPath[PATH_MAX];
        sprintf(binaryShaderPath, "%s/%s", GetEntityPath(shader), profile);
        sprintf(binaryShaderStreamPath, "%s/Stream", binaryShaderPath);

        auto binaryShaderStream = CreateMemoryStream(binaryShaderStreamPath);
        auto binaryShader = CreateBinaryShader(binaryShaderPath);

        SetSourceShader(binaryShader, shader);

        SetBinaryShaderProfile(binaryShader, profile);
        SetBinaryShaderStream(binaryShader, binaryShaderStream);

        return binaryShader;
    }

    static void OnInvalidated(Entity entity, bool before, bool after) {
        if(after && HasStream(entity)) {
            for(auto shader = GetNextEntity(0); IsEntityValid(shader); shader = GetNextEntity(shader)) {
                if(!HasShader(shader)) {
                    continue;
                }

                if(GetShaderSourceStream(shader) == entity ||
                   GetShaderVaryingDefStream(shader) == entity) {
                    SetInvalidated(shader, true);
                }
            }
        }
    }

    static bool ServiceStart() {
        SubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }
