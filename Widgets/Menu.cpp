//
// Created by Kim on 11-02-2019.
//

#include "Menu.h"
#include <Gui/Widget.h>
#include <Json/NativeUtils.h>

BeginUnit(Menu)
    BeginComponent(Menu)
        RegisterBase(Widget)
        ComponentTemplate({
            "$components": ["Component.HorizontalLayout"],
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.PanelStyleMesh}",
            "LayoutChildMinimumSize": [32, 32]
        })
    EndComponent()

    BeginComponent(MainMenu)
        RegisterBase(Widget)
        ComponentTemplate({
              "$components": ["Component.HorizontalLayout"],
              "RenderableSubMesh": "{SceneNodeScene.SceneStyle.PanelStyleMesh}",
              "LayoutChildMinimumSize": [32, 32],
              "LayoutPadding": [3, 3, 3, 3]
          })
    EndComponent()

    BeginComponent(MenuItem)
        RegisterBase(Widget)
        RegisterProperty(StringRef, MenuItemTitle)
    EndComponent()

    BeginComponent(Separator)
        RegisterBase(Widget)
    EndComponent()
EndUnit()