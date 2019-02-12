//
// Created by Kim on 17-01-2019.
//

#include "MainFrame.h"
#include <Rendering/SceneRenderer.h>
#include <Rendering/RenderContext.h>
#include <Scene/Scene.h>
#include <Scene/Camera.h>
#include <Gui/Widget.h>
#include <Json/NativeUtils.h>

BeginUnit(MainFrame)
    BeginComponent(MainFrame)
        RegisterBase(Scene)
        RegisterChildProperty(SceneRenderer, MainFrameRenderer)
        RegisterChildProperty(Camera, MainFrameCamera)

        ComponentTemplate({
            "Uuid": "Template.MainFrame",
            "$components": ["Component.VerticalLayout"],
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
            "SceneStyle": "Style.Default"
        })

    EndComponent()

    BeginComponent(MainFrameStyle)
        RegisterChildProperty(WidgetMesh, MainFrameStyleMesh)
    EndComponent()
EndUnit()