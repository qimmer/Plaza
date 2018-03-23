//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/String.h>
#include <Foundation/Stream.h>
#include "Program.h"
#include "Shader.h"
#include "BinaryShader.h"

struct Program {
    Entity VertexShader, PixelShader;
};

DefineComponent(Program)
    DefineProperty(Entity, VertexShader)
    DefineProperty(Entity, PixelShader)
EndComponent()

DefineComponentPropertyReactive(Program, Entity, VertexShader);
DefineComponentPropertyReactive(Program, Entity, PixelShader);

DefineService(Program)
EndService()

static void OnChanged(Entity binaryShader) {
    auto shader = GetParent(binaryShader);

    if(IsEntityValid(shader) && HasShader(shader)) {
        for_entity(program, Program) {
            if(GetVertexShader(program) == shader || GetPixelShader(program) == shader) {
                FireEvent(ProgramChanged, program);
            }
        }
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

