//
// Created by Kim on 17-01-2019.
//

#include "MainFrame.h"
#include <Rendering/SceneRenderer.h>
#include <Rendering/RenderContext.h>
#include <Scene/Scene.h>
#include <Scene/Camera.h>
#include <Gui/GuiPickRay.h>
#include <Gui/Widget.h>
#include <Json/NativeUtils.h>

BeginUnit(MainFrame)
    BeginComponent(MainFrame)
        RegisterBase(Scene)
        RegisterChildProperty(SceneRenderer, MainFrameRenderer)
        RegisterChildProperty(GuiPickRay, MainFramePicker)
        RegisterChildProperty(Camera, MainFrameCamera)

        ComponentTemplate({
            "Uuid": "Template.MainFrame",
            "LayoutMode": "LayoutMode_Vertical",
            "MainFrameRenderer": {
                "SceneRendererTarget": "Template.MainFrame",
                "SceneRendererScene": "Template.MainFrame",
                "SceneRendererPath": "Gui.RenderPath",
                "SceneRendererCamera": "Template.MainFrame.MainFrameCamera",
                "SceneRendererViewport": [0.0, 0.0, 1.0, 1.0]
            },
            "MainFrameCamera": {
                "FrustumNearClip": -1000.0,
                "FrustumFarClip": 1000.0,
                "OrthographicFrustumTopLeft": [0, 0],
                "OrthographicFrustumBottomRight": "{Owner.MainFrameRenderer.SceneRendererTarget.RenderTargetSize}",
                "Position3D": [0, 0, -100]
            },
            "MainFramePicker": {
                "PickRaySceneRenderer": "{Owner.MainFrameRenderer}",
                "GuiPickRayClickState": {
                    "InputStateContext": "{Owner.Owner}",
                    "InputStateKey": "MOUSEBUTTON_0"
                }
            },
            "LayoutChildOrder": [
                {
                    "LayoutChildOrderingProperty": "Property.Children"
                }
            ],
            "LayoutChildWeight": [1.0, 1.0],
            "SceneStyle": "Style.Default"
        })

    EndComponent()

    BeginComponent(MainFrameStyle)
        RegisterChildProperty(WidgetMesh, MainFrameStyleMesh)
    EndComponent()
EndUnit()