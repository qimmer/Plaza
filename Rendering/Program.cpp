//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/String.h>
#include <Foundation/Stream.h>
#include "Program.h"
#include "Shader.h"
#include "BinaryShader.h"

struct Program {
};

DefineComponent(Program)
    Dependency(Stream)
    Dependency(Hierarchy)
EndComponent()

DefineService(Program)
EndService()

static void OnChanged(Entity entity) {
    auto parent = GetParent(entity);
    auto grandParent = IsEntityValid(parent) ? GetParent(parent) : 0;

    if(IsEntityValid(grandParent) && HasProgram(grandParent))
    {
        FireEvent(ProgramChanged, grandParent);
    }
}

static bool ServiceStart() {
    SubscribeBinaryShaderChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeBinaryShaderChanged(OnChanged);
    return true;
}

