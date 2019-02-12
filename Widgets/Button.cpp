//
// Created by Kim on 17-01-2019.
//

#include "Button.h"
#include <Json/NativeUtils.h>
#include <Gui/Widget.h>
#include <Gui/Font.h>
#include <Widgets/Style.h>

BeginUnit(Button)
    BeginComponent(ButtonStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, ButtonStyleMesh)
        RegisterReferenceProperty(Font, ButtonStyleFont)
        RegisterProperty(v4i, ButtonStylePadding)
    EndComponent()

    BeginComponent(Button)
        RegisterBase(Widget)
        RegisterProperty(StringRef, ButtonTitle)

        ComponentTemplate({
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.ButtonStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.ButtonStylePadding}",
            "LayoutChildMinimumSize": [40, 20],
            "Children": [
                {
                    "TextWidgetFont": "{SceneNodeScene.SceneStyle.ButtonStyleFont}",
                    "TextWidgetAlignment": [0.0, 0.5],
                    "TextWidgetText": "{Owner.ButtonTitle}"
                }
            ]
        })
    EndComponent()
EndUnit()