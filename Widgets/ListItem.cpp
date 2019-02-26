//
// Created by Kim on 25-02-2019.
//

#include "ListItem.h"
#include <Json/NativeUtils.h>
#include <Gui/Widget.h>
#include <Gui/Layout.h>
#include <Gui/Font.h>
#include <Widgets/Style.h>

BeginUnit(ListItem)
    BeginComponent(ListItemStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, ListItemStyleMesh)
        RegisterProperty(v4i, ListItemStylePadding)
    EndComponent()

    BeginComponent(ListItem)
        RegisterBase(Widget)
        RegisterChildProperty(Widget, ListItemLabel)
        RegisterChildProperty(Layout, ListItemChildContainer)
        RegisterProperty(StringRef, ListItemTitle)
        ComponentTemplate({
            "LayoutMode": "LayoutMode_Vertical",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.ListItemStylePadding}",
            "LayoutChildOrder": [
                {
                    "LayoutChildOrderingProperty": "Property.ListItemLabel",
                    "LayoutChildOrderingProperty": "Property.ListItemChildContainer"
                }
            ],
            "ListItemLabel": {
                "$components": ["Component.InteractableWidget"],
                "LayoutMode": "LayoutMode_Horizontal",
                "RenderableSubMesh": "{SceneNodeScene.SceneStyle.ListItemStyleMesh}",
                "Children": [
                    {
                        "TextWidgetFont": "{SceneNodeScene.SceneStyle.ButtonStyleFont}",
                        "TextWidgetText": "{Owner.Owner.ListItemTitle}",
                        "TextWidgetColor": "{Owner.WidgetStateColor}"
                    }
                ]
            },
            "ListItemChildContainer": {
                "LayoutMode": "LayoutMode_Vertical"
            }
        })
    EndComponent()
EndUnit()