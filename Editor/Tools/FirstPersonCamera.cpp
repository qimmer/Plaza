//
// Created by Kim Johannsen on 20-03-2018.
//

#include <Core/Entity.h>
#include <Input/InputState.h>
#include <Rendering/Context.h>
#include <Editor/Editors/SceneEditor.h>
#include <Logic/State.h>
#include <Scene3D/Transform3D.h>
#include "FirstPersonCamera.h"
#include <cglm/cglm.h>
#include <Foundation/AppLoop.h>

DefineService(FirstPersonCameraTool)
EndService()

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

static void OnUpdate(double deltaTime) {
    auto camera = GetCurrentSceneEditorCamera();
    if(IsEntityValid(camera)) {
        v3f moveDelta = {
            -GetStateValue(LeftInputState) + GetStateValue(RightInputState),
            -GetStateValue(DownInputState) + GetStateValue(UpInputState),
            -GetStateValue(BackInputState) + GetStateValue(ForwardInputState),
        };

        v3f rotateDelta = {
                GetStateValue(LookDownInputState) - GetStateValue(LookUpInputState),
                -GetStateValue(LookLeftInputState) + GetStateValue(LookRightInputState),
                0.0f
        };

        auto position = GetPosition3D(camera);
        auto euler = GetRotationEuler3D(camera);

        auto rotateFactor = GetStateValue(ActivateInputState);

        euler.x += rotateDelta.x * rotateFactor;
        euler.y += rotateDelta.y * rotateFactor;
        euler.z = 0.0f;

        euler.x = glm_max(glm_min(euler.x, 89.0f), -89.0f);

        float yUp[] = {0.0f, 1.0f, 0.0f};
        float xUp[] = {1.0f, 0.0f, 0.0f};

        glm_vec_rotate(&moveDelta.x, glm_rad(euler.x), xUp);
        glm_vec_rotate(&moveDelta.x, glm_rad(euler.y), yUp);

        position.x += moveDelta.x * deltaTime;
        position.y += moveDelta.y * deltaTime;
        position.z += moveDelta.z * deltaTime;

        SetPosition3D(camera, position);
        SetRotationEuler3D(camera, euler);
    }
}

static bool ServiceStart() {
    auto editorRoot = CreateHierarchy(0, ".editor");
    auto root = CreateHierarchy(editorRoot, "InputBindings");
    
    ActivateInputState = CreateInputState(root, "FpsActivate");
    ForwardInputState = CreateInputState(root, "FpsForward");
    BackInputState = CreateInputState(root, "FpsBack");
    LeftInputState = CreateInputState(root, "FpsLeft");
    RightInputState = CreateInputState(root, "FpsRight");
    UpInputState = CreateInputState(root, "FpsUp");
    DownInputState = CreateInputState(root, "FpsDown");
    LookLeftInputState = CreateInputState(root, "FpsLookLeft");
    LookRightInputState = CreateInputState(root, "FpsLookRight");
    LookUpInputState = CreateInputState(root, "FpsLookUp");
    LookDownInputState = CreateInputState(root, "FpsLookDown");

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

    SubscribeAppUpdate(OnUpdate);

    return true;
}

static bool ServiceStop() {
    UnsubscribeAppUpdate(OnUpdate);

    return true;
}