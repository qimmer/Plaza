//
// Created by Kim Johannsen on 20-03-2018.
//

#include <Core/Entity.h>
#include <Input/InputState.h>
#include <Rendering/Context.h>
#include <Editor/Editors/SceneEditor.h>
#include <Logic/State.h>
#include <Scene3D/Transform3D.h>
#include "FirstPersonCameraTool.h"
#include <cglm/cglm.h>
#include <Foundation/AppLoop.h>
#include <Input/InputState.h>
#include <Input/Key.h>

static Entity
    ActivateInputState,
    ForwardInputState,
    BackInputState,
    LeftInputState,
    RightInputState,
    UpInputState,
    DownInputState,
    LookLeftInputState,
    LookRightInputState,
    LookUpInputState,
    LookDownInputState;

static void OnServiceStart(Service service) {
    auto editorRoot = CreateHierarchy(0, ".editor");
    auto root = CreateHierarchy(editorRoot, "InputBindings");
    
    ActivateInputState = CreateState(root, "FpsActivate");
    ForwardInputState = CreateState(root, "FpsForward");
    BackInputState = CreateState(root, "FpsBack");
    LeftInputState = CreateState(root, "FpsLeft");
    RightInputState = CreateState(root, "FpsRight");
    UpInputState = CreateState(root, "FpsUp");
    DownInputState = CreateState(root, "FpsDown");
    LookLeftInputState = CreateState(root, "FpsLookLeft");
    LookRightInputState = CreateState(root, "FpsLookRight");
    LookUpInputState = CreateState(root, "FpsLookUp");
    LookDownInputState = CreateState(root, "FpsLookDown");

    SetInputStateKey(ActivateInputState, MOUSEBUTTON_1);
    SetInputStateKey(ForwardInputState, KEY_W);
    SetInputStateKey(BackInputState, KEY_S);
    SetInputStateKey(LeftInputState, KEY_A);
    SetInputStateKey(RightInputState, KEY_D);
    SetInputStateKey(UpInputState, KEY_E);
    SetInputStateKey(DownInputState, KEY_Q);
    SetInputStateKey(LookLeftInputState, MOUSE_LEFT);
    SetInputStateKey(LookRightInputState, MOUSE_RIGHT);
    SetInputStateKey(LookUpInputState, MOUSE_UP);
    SetInputStateKey(LookDownInputState, MOUSE_DOWN);

    SetInputStateStateScale(LookLeftInputState, 0.25f);
    SetInputStateStateScale(LookRightInputState, 0.25f);
    SetInputStateStateScale(LookUpInputState, 0.25f);
    SetInputStateStateScale(LookDownInputState, 0.25f);

    SetInputStateStateScale(ForwardInputState, 10.0f);
    SetInputStateStateScale(BackInputState, 10.0f);
    SetInputStateStateScale(LeftInputState, 10.0f);
    SetInputStateStateScale(RightInputState, 10.0f);
    SetInputStateStateScale(UpInputState, 10.0f);
    SetInputStateStateScale(DownInputState, 10.0f);
}

DefineService(FirstPersonCameraTool)
        Subscribe(FirstPersonCameraToolStarted, OnServiceStart)
EndService()
