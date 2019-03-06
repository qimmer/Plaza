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
        RegisterBase(Widget)
        ComponentTemplate({
            "$components": ["Component.Widget"],
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.PanelStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.PanelStylePadding}",
              "LayoutChildOrder": [
              {
                  "LayoutChildOrderingProperty": "Property.Children"
              }
              ]
        })
    EndComponent()

    BeginComponent(WellStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, WellStyleMesh)
        RegisterProperty(v4i, WellStylePadding)
    EndComponent()

    BeginComponent(Well)
        RegisterBase(Widget)
        ComponentTemplate({
            "$components": ["Component.Widget"],
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.WellStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.WellStylePadding}",
              "LayoutChildOrder": [
              {
                  "LayoutChildOrderingProperty": "Property.Children"
              }
              ]
        })
    EndComponent()

    BeginComponent(ItemWellStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, ItemWellStyleMesh)
        RegisterProperty(v4i, ItemWellStylePadding)
    EndComponent()

    BeginComponent(ItemWell)
        RegisterBase(Widget)
        ComponentTemplate({
            "$components": ["Component.Widget"],
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.ItemWellStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.ItemWellStylePadding}",
              "LayoutChildOrder": [
              {
                  "LayoutChildOrderingProperty": "Property.Children"
              }
              ]
        })
    EndComponent()
EndUnit()