//
// Created by Kim on 17-01-2019.
//

#include "MainFrame.h"
#include <Rendering/SceneRenderer.h>
#include <Rendering/RenderContext.h>
#include <Scene/Scene.h>
#include <Scene/Camera.h>
#include <Gui/GuiController.h>
#include <Gui/Widget.h>
#include <Json/NativeUtils.h>

BeginUnit(MainFrame)
    BeginComponent(MainFrame)
        RegisterBase(Scene)
        BeginChildProperty(MainFrameRenderer)
        EndChildProperty()
        BeginChildProperty(MainFrameController)
        EndChildProperty()
        BeginChildProperty(MainFrameCamera)

        EndChildProperty()
    EndComponent()

    BeginComponent(MainFrameStyle)
        BeginChildProperty(MainFrameStyleMesh)
    EndChildProperty()
    EndComponent()

    BeginPrefab(MainFrame)
        PrefabJson({
                  "LayoutMode": "LayoutMode_Vertical",
                "MainFrameRenderer": {
                    "SceneRendererTarget": "{Owner}",
                            "SceneRendererScene": "{Owner}",
                            "SceneRendererPath": "Gui.RenderPath",
                            "SceneRendererCamera": "{Owner.MainFrameCamera}",
                            "SceneRendererViewport": [0.0, 0.0, 1.0, 1.0]
                },
                "MainFrameCamera": {
                    "FrustumNearClip": -1000.0,
                            "FrustumFarClip": 1000.0,
                            "OrthographicFrustumTopLeft": [0, 0],
                    "OrthographicFrustumBottomRight": "{Owner.MainFrameRenderer.SceneRendererTarget.RenderTargetSize}",
                            "Position3D": [0, 0, -100]
                },
                "InputMappings": [
                {
                    "Name": "LeftClick",
                            "InputStateContext": "{Owner}",
                            "InputStateKey": "MOUSEBUTTON_0"
                },
                {
                    "Name": "RightClick",
                            "InputStateContext": "{Owner}",
                            "InputStateKey": "MOUSEBUTTON_1"
                },
                {
                    "Name": "ScrollUp",
                            "InputStateContext": "{Owner}",
                            "InputStateKey": "MOUSE_SCROLL_UP"
                },
                {
                    "Name": "ScrollDown",
                            "InputStateContext": "{Owner}",
                            "InputStateKey": "MOUSE_SCROLL_DOWN"
                }
                ],
                "MainFrameController": {
                    "GuiControllerWidgetPicker": {
                        "PickRaySceneRenderer": "{Owner.Owner.MainFrameRenderer}"
                    },
                    "GuiControllerScrollablePicker": {
                        "PickRaySceneRenderer": "{Owner.Owner.MainFrameRenderer}"
                    },
                    "GuiControllerLeftClicked": "{Owner.InputMappings[LeftClick].InputStateValue}",
                            "GuiControllerRightClicked": "{Owner.InputMappings[RightClick].InputStateValue}",
                            "GuiControllerScrollDown": "{Owner.InputMappings[ScrollDown].InputStateValue}",
                            "GuiControllerScrollUp": "{Owner.InputMappings[ScrollUp].InputStateValue}"
                },
                "LayoutChildWeight": [1.0, 1.0],
                "LayoutChildOrder": [
                {
                    "LayoutChildOrderingProperty": "Property.Children"
                }
                ]
          })

    EndPrefab()
EndUnit()