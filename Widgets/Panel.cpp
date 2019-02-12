//
// Created by Kim on 17-01-2019.
//

#include "Panel.h"
#include <Json/NativeUtils.h>
#include <Gui/Widget.h>
#include <Gui/Font.h>
#include <Widgets/Style.h>

BeginUnit(Panel)
    BeginComponent(PanelStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, PanelStyleMesh)
        RegisterProperty(v4i, PanelStylePadding)
    EndComponent()

    BeginComponent(Panel)
        ComponentTemplate({
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.PanelStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.PanelStylePadding}"
        })
    EndComponent()

    BeginComponent(WellStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, WellStyleMesh)
        RegisterProperty(v4i, WellStylePadding)
    EndComponent()

    BeginComponent(Panel)
        ComponentTemplate({
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.WellStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.WellStylePadding}"
        })
    EndComponent()
EndUnit()