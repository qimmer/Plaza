//
// Created by Kim on 17-01-2019.
//

#include "Button.h"
#include <Json/NativeUtils.h>
#include <Gui/Widget.h>
#include <Gui/Font.h>
#include <Gui/TextWidget.h>
#include <Widgets/Style.h>

BeginUnit(Button)
    BeginComponent(ButtonStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, ButtonStyleMesh)
        RegisterReferenceProperty(Font, ButtonStyleFont)
        RegisterProperty(v4f, ButtonStyleTextColor)
        RegisterProperty(v4i, ButtonStylePadding)
    EndComponent()

    BeginComponent(Button)
        RegisterBase(InteractableWidget)
        RegisterProperty(StringRef, ButtonTitle)
        RegisterChildProperty(TextWidget, ButtonLabel)

        ComponentTemplate({
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.ButtonStyleMesh}",
            "LayoutMode": "LayoutMode_Horizontal",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.ButtonStylePadding}",
            "LayoutChildOrder": [
              {
                  "LayoutChildOrderingProperty": "Property.ButtonLabel"
              }
            ],
            "ButtonLabel": {
                "TextWidgetFont": "{SceneNodeScene.SceneStyle.ButtonStyleFont}",
                "TextWidgetText": "{Owner.ButtonTitle}",
                "TextWidgetColor": "{Owner.WidgetStateColor}"
            }
        })
    EndComponent()
EndUnit()