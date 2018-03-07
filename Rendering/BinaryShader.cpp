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
        Entity SourceShader;
        String BinaryShaderProfile;
    };

    DefineComponent(BinaryShader)
        Dependency(Invalidation)
        Dependency(Stream)
        DefineProperty(Entity, BinaryShaderProfile)
    EndComponent()

    DefineComponentProperty(BinaryShader, Entity, SourceShader)
    DefineComponentProperty(BinaryShader, StringRef, BinaryShaderProfile)

    DefineService(BinaryShader)
        ServiceSetting(SupportedShaderProfiles, SETTING_SUPPORTED_SHADER_PROFILES)
    EndService()

#ifdef __APPLE__
    String SupportedShaderProfiles = "osx_410";
#endif

#ifdef WIN32
    String SupportedShaderProfiles = "windows_xs_4_0";
#endif

    void SetSupportedShaderProfiles(StringRef profiles) {
        SupportedShaderProfiles = profiles;
    }

    StringRef GetSupportedShaderProfiles() {
        return SupportedShaderProfiles.c_str();
    }

    static bool ServiceStart() {
        return true;
    }

    static bool ServiceStop() {
        return true;
    }

