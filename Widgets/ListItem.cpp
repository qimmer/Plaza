//
// Created by Kim on 25-02-2019.
//

#include "ListItem.h"
#include <Json/NativeUtils.h>
#include <Gui/Widget.h>
#include <Gui/Layout.h>
#include <Gui/Font.h>
#include <Widgets/Style.h>

LocalFunction(OnWidgetClickedChanged, void, Entity listItemContainer, bool oldValue, bool newValue) {
    auto listItem = GetOwner(listItemContainer);
    if(newValue && HasComponent(listItem, ComponentOf_ListItem())) {
        SetListItemExpanded(listItem, !GetListItemExpanded(listItem));
    }
}

BeginUnit(ListItem)
    BeginComponent(ListItemStyle)
        RegisterBase(Style)
        RegisterChildProperty(WidgetMesh, ListItemStyleMesh)
        RegisterProperty(v4i, ListItemStylePadding)
    EndComponent()

    BeginComponent(ListItem)
        RegisterBase(Widget)
        RegisterChildProperty(Layout, ListItemContainer)
        RegisterProperty(StringRef, ListItemTitle)
        RegisterProperty(StringRef, ListItemIcon)
        RegisterProperty(bool, ListItemExpanded)
        ComponentTemplate({
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
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClicked()), OnWidgetClickedChanged, 0)
EndUnit()