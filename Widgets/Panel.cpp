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
        BeginChildProperty(PanelStyleMesh)
        EndChildProperty()
        RegisterProperty(v4i, PanelStylePadding)
    EndComponent()

    BeginComponent(WellStyle)
        RegisterBase(Style)
        BeginChildProperty(WellStyleMesh)
        EndChildProperty()
        RegisterProperty(v4i, WellStylePadding)
    EndComponent()

    BeginComponent(ItemWellStyle)
        RegisterBase(Style)
        BeginChildProperty(ItemWellStyleMesh)
        EndChildProperty()
        RegisterProperty(v4i, ItemWellStylePadding)
    EndComponent()

    BeginPrefab(Panel)
        PrefabJson({
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

    BeginPrefab(Well)
        PrefabJson({
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

    BeginPrefab(ItemWell)
        PrefabJson({
            "$components": ["Component.Widget", "Component.ScrollableLayout"],
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