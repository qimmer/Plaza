//
// Created by Kim Johannsen on 20-03-2018.
//

#include <Core/Entity.h>
#include <Input/InputState.h>
#include <Rendering/RenderContext.h>
#include <Editor/Editors/SceneEditor.h>
#include <Scene3D/Transform3D.h>
#include "FirstPersonCameraTool.h"
#include <cglm/cglm.h>
#include <Foundation/AppLoop.h>
#include <Input/InputState.h>
#include <Input/Key.h>

LocalFunction(OnServiceStart, void, Service service) {
    auto editorRoot = CreateNode(0, ".editor");
    auto root = CreateNode(editorRoot, "InputBindings");
}

DefineService(FirstPersonCameraTool)
        RegisterSubscription(FirstPersonCameraToolStarted, OnServiceStart, 0)
EndService()
