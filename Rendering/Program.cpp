//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Invalidation.h>
#include <Core/String.h>
#include <Foundation/Stream.h>
#include "Program.h"
#include "Shader.h"
#include "BinaryShader.h"


    struct Program {
        Program() {
            memset(this, 0, sizeof(Program));
        }
    };

    String ShadingProfile;

    DefineComponent(Program)
        Dependency(Invalidation)
        Dependency(Stream)
        Dependency(Hierarchy)
    EndComponent()

    DefineService(Program)
        ServiceSetting(ShadingProfile, SETTING_SHADING_PROFILE)
    EndService()

    void SetShadingProfile(StringRef profile) {
        ShadingProfile = profile;
    }

    StringRef GetShadingProfile() {
        return ShadingProfile.c_str();
    }

    static void OnInvalidated(Entity entity, bool before, bool after) {
        auto parent = GetParent(entity);
        auto grandParent = IsEntityValid(parent) ? GetParent(parent) : 0;

        if(IsEntityValid(grandParent) && HasProgram(grandParent))
        {
            SetInvalidated(grandParent, true);
        }
    }

    static bool ServiceStart() {
        char profile[64];
        char profiles[256];
        strcpy(profiles, GetSupportedShaderProfiles());
        strcpy(profile, strtok (profiles, " ;"));

        SetShadingProfile(profile);
        SubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }

