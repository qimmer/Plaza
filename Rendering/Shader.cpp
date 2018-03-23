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
EndComponent()

DefineComponentPropertyReactive(Shader, u8, ShaderType)
DefineComponentPropertyReactive(Shader, Entity, ShaderDeclaration)

DefineService(Shader)
EndService()

Entity GetBinaryShader(Entity shader, u8 profile) {
    for(auto binaryShader = GetFirstChild(shader); binaryShader; binaryShader = GetSibling(binaryShader)) {
        if(!HasBinaryShader(binaryShader)) continue;
        if(GetBinaryShaderProfile(binaryShader) == profile) {
            return binaryShader;
        }
    }

    auto flagIndex = GetEnumFlagIndexByValue(EnumOf_ShaderProfile(), profile);
    auto binaryShader = CreateBinaryShader(shader, GetEnumFlagName(EnumOf_ShaderProfile(), flagIndex));
    SetSourceShader(binaryShader, shader);
    SetBinaryShaderProfile(binaryShader, profile);

    return binaryShader;
}

static void OnChanged(Entity entity) {
    if(HasBinaryShader(entity)) {
        auto shader = GetSourceShader(entity);
        if(IsEntityValid(shader) && HasShader(shader)) {
            for_entity(program, Program) {
                if(GetVertexShader(program) == shader || GetPixelShader(program) == shader) {
                    FireEvent(ProgramChanged, program);
                    break;
                }
            }

        }
    }

    if(HasShader(entity)) {
        for_entity(program, Program) {
            if(GetVertexShader(program) == entity || GetPixelShader(program) == entity) {
                FireEvent(ProgramChanged, program);

                for_entity(binaryShader, BinaryShader) {
                    if(GetSourceShader(binaryShader) == entity) {
                        FireEvent(BinaryShaderChanged, binaryShader);
                        break;
                    }
                }

                break;
            }
        }
    }
}

static bool ServiceStart() {
    SubscribeStreamChanged(OnChanged);
    SubscribeStreamContentChanged(OnChanged);
    SubscribeBinaryShaderChanged(OnChanged);
    SubscribeShaderChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeStreamChanged(OnChanged);
    UnsubscribeStreamContentChanged(OnChanged);
    UnsubscribeBinaryShaderChanged(OnChanged);
    UnsubscribeShaderChanged(OnChanged);
    return true;
}

