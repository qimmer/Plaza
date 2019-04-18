//
// Created by Kim on 17-01-2019.
//

#include "Dock.h"
#include <Json/JsonBinding.h>
#include <Json/NativeUtils.h>
#include <Gui/Widget.h>
#include <Gui/Font.h>
#include <Widgets/Style.h>
#include <Rendering/Renderable.h>
#include <Gui/Layout.h>

BeginUnit(Dock)
    BeginComponent(DockStyle)
        RegisterBase(Style)
        BeginChildProperty(DockStyleMesh)
        EndChildProperty()
        RegisterReferenceProperty(Font, DockStyleFont)
        RegisterProperty(v4i, DockStylePadding)
    EndComponent()

    BeginComponent(Dock)
        RegisterProperty(StringRef, DockTitle)
    EndComponent()

    BeginPrefab(Dock)
        PrefabJson({
           "RenderableSubMesh": "{SceneNodeScene.SceneStyle.DockStyleMesh}",
           "LayoutPadding": "{SceneNodeScene.SceneStyle.DockStylePadding}"
        })
    EndPrefab()
EndUnit()