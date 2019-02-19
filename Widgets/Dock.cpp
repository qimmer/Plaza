//
// Created by Kim on 17-01-2019.
//

#include "Dock.h"
#include <Json/NativeUtils.h>
#include <Gui/Widget.h>
#include <Gui/Font.h>
#include <Widgets/Style.h>

BeginUnit(Dock)
    BeginComponent(DockStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, DockStyleMesh)
        RegisterReferenceProperty(Font, DockStyleFont)
        RegisterProperty(v4i, DockStylePadding)
    EndComponent()

    BeginComponent(Dock)
        RegisterProperty(StringRef, DockTitle)

        ComponentTemplate({
              "RenderableSubMesh": "{SceneNodeScene.SceneStyle.DockStyleMesh}",
              "LayoutPadding": "{SceneNodeScene.SceneStyle.DockStylePadding}"
          })
    EndComponent()
EndUnit()