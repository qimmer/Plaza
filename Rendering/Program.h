//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_PROGRAM_H
#define PLAZA_PROGRAM_H

#include <Core/Entity.h>


    DeclareComponent(Program)

    DeclareService(Program)

    void SetShadingProfile(StringRef profile);

    StringRef GetShadingProfile();

#define SETTING_SHADING_PROFILE "shading_profile"

#endif //PLAZA_PROGRAM_H
