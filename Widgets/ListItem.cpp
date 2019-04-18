//
// Created by Kim on 25-02-2019.
//

#include "ListItem.h"
#include <Json/NativeUtils.h>
#include <Gui/Widget.h>
#include <Gui/Layout.h>
#include <Gui/Font.h>
#include <Widgets/Style.h>

static void OnInteractableWidgetChanged(Entity listItemContainer, const InteractableWidget& oldValue, const InteractableWidget& newValue) {
    auto listItem = GetOwnership(listItemContainer).Owner;
    if(!oldValue.WidgetClicked && newValue.WidgetClicked && HasComponent(listItem, ComponentOf_ListItem())) {
        auto listItemData = GetListItem(listItem);
        listItemData.ListItemExpanded = !listItemData.ListItemExpanded;
        SetListItem(listItem, listItemData);
    }
}

BeginUnit(ListItem)
    BeginComponent(ListItemStyle)
        RegisterBase(Style)
        BeginChildProperty(ListItemStyleMesh)
        EndChildProperty()
        RegisterProperty(v4i, ListItemStylePadding)
    EndComponent()

    BeginComponent(ListItem)
        RegisterBase(Widget)
        BeginChildProperty(ListItemContainer)
        EndChildProperty()
        RegisterProperty(StringRef, ListItemTitle)
        RegisterProperty(StringRef, ListItemIcon)
        RegisterProperty(bool, ListItemExpanded)

    EndComponent()

    BeginPrefab(ListItem)
        PrefabJson({
            "LayoutMode": "LayoutMode_Vertical",
            "LayoutPadding": [10, 0, 0, 0],
            "LayoutChildWeight": [1, 0],
            "LayoutChildOrder": [
                {
                    "LayoutChildOrderingProperty": "Property.ListItemContainer"
                },
                {
                    "LayoutChildOrderingProperty": "Property.Children"
                }
            ],
            "ListItemContainer": {
                "$components": ["Component.InteractableWidget"],
                "LayoutMode": "LayoutMode_Horizontal",
                "LayoutPadding": "{SceneNodeScene.SceneStyle.ListItemStylePadding}",
                "LayoutChildWeight": [1, 0],
                "LayoutChildOrder": [
                {
                    "LayoutChildOrderingProperty": "Property.Children"
                }
                ],
                "Children": [
                    {
                        "TextWidgetFont": "{SceneNodeScene.SceneStyle.StyleIconFont}",
                        "TextWidgetText": "\uf0d7",
                        "TextWidgetColor": "{Owner.WidgetStateColor}"
                    },
                    {
                        "TextWidgetFont": "{SceneNodeScene.SceneStyle.StyleIconFont}",
                        "TextWidgetText": "{Owner.Owner.ListItemIcon}",
                        "TextWidgetColor": "{Owner.WidgetStateColor}"
                    },
                    {
                        "TextWidgetFont": "{SceneNodeScene.SceneStyle.ButtonStyleFont}",
                        "TextWidgetText": "{Owner.Owner.ListItemTitle}",
                        "TextWidgetColor": "{Owner.WidgetStateColor}"
                    }
                ],
                "RenderableSubMesh": "{SceneNodeScene.SceneStyle.ListItemStyleMesh}"
            }
        })
    EndPrefab()

    RegisterSystem(OnInteractableWidgetChanged, ComponentOf_InteractableWidget())
EndUnit()