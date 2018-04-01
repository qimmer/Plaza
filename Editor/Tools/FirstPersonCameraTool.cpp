//
// Created by Kim Johannsen on 20-03-2018.
//

#include <Core/Entity.h>
#include <Input/State.h>
#include <Rendering/Context.h>
#include <Editor/Editors/SceneEditor.h>
#include <Logic/State.h>
#include <Scene3D/Transform3D.h>
#include "FirstPersonCameraTool.h"
#include <cglm/cglm.h>
#include <Foundation/AppLoop.h>

static void OnServiceStart() {
    auto editorRoot = CreateHierarchy(0, ".editor");
    auto root = CreateHierarchy(editorRoot, "InputBindings");
    
    ActivateState = CreateState(root, "FpsActivate");
    ForwardState = CreateState(root, "FpsForward");
    BackState = CreateState(root, "FpsBack");
    LeftState = CreateState(root, "FpsLeft");
    RightState = CreateState(root, "FpsRight");
    UpState = CreateState(root, "FpsUp");
    DownState = CreateState(root, "FpsDown");
    LookLeftState = CreateState(root, "FpsLookLeft");
    LookRightState = CreateState(root, "FpsLookRight");
    LookUpState = CreateState(root, "FpsLookUp");
    LookDownState = CreateState(root, "FpsLookDown");

    SetStateKey(ActivateState, MOUSEBUTTON_1);
    SetStateKey(ForwardState, KEY_W);
    SetStateKey(BackState, KEY_S);
    SetStateKey(LeftState, KEY_A);
    SetStateKey(RightState, KEY_D);
    SetStateKey(UpState, KEY_E);
    SetStateKey(DownState, KEY_Q);
    SetStateKey(LookLeftState, MOUSE_LEFT);
    SetStateKey(LookRightState, MOUSE_RIGHT);
    SetStateKey(LookUpState, MOUSE_UP);
    SetStateKey(LookDownState, MOUSE_DOWN);

    SetStateStateScale(LookLeftState, 0.25f);
    SetStateStateScale(LookRightState, 0.25f);
    SetStateStateScale(LookUpState, 0.25f);
    SetStateStateScale(LookDownState, 0.25f);

    SetStateStateScale(ForwardState, 10.0f);
    SetStateStateScale(BackState, 10.0f);
    SetStateStateScale(LeftState, 10.0f);
    SetStateStateScale(RightState, 10.0f);
    SetStateStateScale(UpState, 10.0f);
    SetStateStateScale(DownState, 10.0f);
}

DefineService(FirstPersonCameraTool)
        Subscribe(AppUpdate, OnUpdate)
        Subscribe(FirstPersonCameraToolStarted, OnServiceStart)
EndService()
