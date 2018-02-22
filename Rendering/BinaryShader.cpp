//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/String.h>
#include <Foundation/Invalidation.h>
#include <Foundation/Stream.h>
#include "BinaryShader.h"


    struct BinaryShader {
        BinaryShader() {
            memset(this, 0, sizeof(BinaryShader));
        }
        Entity BinaryShaderStream, SourceShader;
        String BinaryShaderProfile;
    };

    DefineComponent(BinaryShader)
            Dependency(Invalidation)
            DefineProperty(Entity, BinaryShaderStream)
            DefineProperty(Entity, BinaryShaderProfile)
    EndComponent()

    DefineComponentProperty(BinaryShader, Entity, SourceShader)
    DefineComponentProperty(BinaryShader, Entity, BinaryShaderStream)
    DefineComponentProperty(BinaryShader, StringRef, BinaryShaderProfile)

    DefineService(BinaryShader)
        ServiceSetting(SupportedShaderProfiles, SETTING_SUPPORTED_SHADER_PROFILES)
    EndService()

    String SupportedShaderProfiles = "osx_410";

    void SetSupportedShaderProfiles(StringRef profiles) {
        SupportedShaderProfiles = profiles;
    }

    StringRef GetSupportedShaderProfiles() {
        return SupportedShaderProfiles.c_str();
    }

    static void OnInvalidated(Entity entity, bool before, bool after) {
        if(after && HasStream(entity)) {
            for(auto binaryShader = GetNextEntity(0); IsEntityValid(binaryShader); binaryShader = GetNextEntity(binaryShader)) {
                if(!HasBinaryShader(binaryShader)) {
                    continue;
                }

                if(GetBinaryShaderStream(binaryShader) == entity) {
                    SetInvalidated(binaryShader, true);
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

