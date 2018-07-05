//
// Created by Kim Johannsen on 20-03-2018.
//

#include <Core/Entity.h>
#include <Input/InputState.h>
#include <Rendering/Context.h>
#include <Editor/Editors/SceneEditor.h>
#include <Scene3D/Transform3D.h>
#include "FirstPersonCameraTool.h"
#include <cglm/cglm.h>
#include <Foundation/AppLoop.h>
#include <Input/InputState.h>
#include <Input/Key.h>

static void OnServiceStart(Service service) {
    auto editorRoot = CreateHierarchy(0, ".editor");
    auto root = CreateHierarchy(editorRoot, "InputBindings");
}

DefineService(FirstPersonCameraTool)
        Subscribe(FirstPersonCameraToolStarted, OnServiceStart)
EndService()
