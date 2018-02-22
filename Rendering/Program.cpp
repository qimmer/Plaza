//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Invalidation.h>
#include <Core/String.h>
#include "Program.h"
#include "Shader.h"
#include "BinaryShader.h"


    struct Program {
        Program() {
            memset(this, 0, sizeof(Program));
        }
        Entity VertexShader;
        Entity PixelShader;
    };

    String ShadingProfile = "osx_410";

    DefineComponent(Program)
        Dependency(Invalidation)
    EndComponent()

    DefineService(Program)
        ServiceSetting(ShadingProfile, SETTING_SHADING_PROFILE)
    EndService()

    DefineComponentProperty(Program, Entity, VertexShader)
    DefineComponentProperty(Program, Entity, PixelShader)

    void SetShadingProfile(StringRef profile) {
        ShadingProfile = profile;
    }

    StringRef GetShadingProfile() {
        return ShadingProfile.c_str();
    }

    static void OnInvalidated(Entity entity, bool before, bool after) {
        if(after && HasBinaryShader(entity)) {
            for(auto program = GetNextEntity(0); IsEntityValid(program); program = GetNextEntity(program)) {
                if(!HasProgram(program)) {
                    continue;
                }

                if(GetVertexShader(program) == GetSourceShader(entity) ||
                   GetPixelShader(program) == GetSourceShader(entity) ) {
                    SetInvalidated(program, true);
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

