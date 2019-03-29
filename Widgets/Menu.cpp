//
// Created by Kim on 11-02-2019.
//

#include "Menu.h"
#include <Gui/Widget.h>
#include <Gui/Layout.h>
#include <Gui/TextWidget.h>
#include <Scene/Transform.h>
#include <Json/NativeUtils.h>
#include <Foundation/Visibility.h>
#include <Foundation/AppNode.h>

static void RepositionMenu(Entity menu, bool vertical) {
    if(HasComponent(menu, ComponentOf_Menu()) && !HasComponent(menu, ComponentOf_MainMenu())) {
        auto itemSize = GetSize2D(GetOwnership(menu).Owner);
        if(vertical) {
            SetPosition3D(menu, {0.0f, (float)itemSize.y, -GetTransformHierarchyLevel(menu) - GetWidgetDepthOrder(menu)});
        }
        else {
            SetPosition3D(menu, {(float)itemSize.x, 0.0f, -GetTransformHierarchyLevel(menu) - GetWidgetDepthOrder(menu)});
        }
    }
}

static void RepositionMenuItemMenu(Entity item) {
    if(HasComponent(item, ComponentOf_MainMenuItem())) {
        auto menu = GetMenuItemSubMenu(item);
        RepositionMenu(menu, true);
    }
    else if(HasComponent(item, ComponentOf_MenuItem())) {
        auto menu = GetMenuItemSubMenu(item);
        RepositionMenu(menu, false);
    }
}

LocalFunction(OnSize2DChanged, void, Entity entity) {
    RepositionMenuItemMenu(entity);
}

LocalFunction(OnMenuChanged, void, Entity entity) {
    RepositionMenu(entity, HasComponent(GetOwnership(entity).Owner, ComponentOf_MainMenuItem()));
}

LocalFunction(OnClickedChanged, void, Entity entity, bool oldClicked, bool newClicked) {
    if(newClicked && HasComponent(entity, ComponentOf_MainMenuItem())) {
        auto menu = GetOwnership(entity).Owner;

        auto& menuItems = GetChildren(menu);
        auto alreadySelected = GetWidgetSelected(entity);
        for(auto i = 0; i < menuItems.size(); ++i) {
            auto selected = menuItems[i] == entity && !alreadySelected;
            auto subMenu = GetMenuItemSubMenu(menuItems[i]);
            SetHidden(subMenu, !selected);
            SetWidgetSelected(menuItems[i], selected);
        }
    }
}

LocalFunction(OnHoveredChanged, void, Entity entity, bool oldHovered, bool newHovered) {
    if(newHovered && HasComponent(entity, ComponentOf_MenuItem())) {
        auto menu = GetOwnership(entity).Owner;
        auto& menuItems = GetChildren(menu);

        bool anyMenuOpen = false;
        for(auto i = 0; i < menuItems.size(); ++i) {
            if(GetWidgetSelected(menuItems[i])) {
                anyMenuOpen = true;
                break;
            }
        }

        if(!anyMenuOpen) return;

        for(auto i = 0; i < menuItems.size(); ++i) {
            auto selected = menuItems[i] == entity;
            auto subMenu = GetMenuItemSubMenu(menuItems[i]);
            SetHidden(subMenu, !selected);
            SetWidgetSelected(menuItems[i], selected);
        }
    }
}

BeginUnit(Menu)
    BeginComponent(MenuItemStyle)
        RegisterChildProperty(WidgetMesh, MenuItemStyleMesh)
        RegisterProperty(v4i, MenuItemStylePadding)
    EndComponent()

    BeginComponent(MainMenuItemStyle)
        RegisterChildProperty(WidgetMesh, MainMenuItemStyleMesh)
    EndComponent()

    BeginComponent(MainMenuStyle)
        RegisterProperty(v4i, MainMenuStylePadding)
    EndComponent()

    BeginComponent(SeparatorStyle)
        RegisterProperty(v4i, SeparatorStylePadding)
        RegisterChildProperty(WidgetMesh, SeparatorStyleVerticalMesh)
        RegisterChildProperty(WidgetMesh, SeparatorStyleHorizontalMesh)
    EndComponent()

    BeginComponent(Menu)
        RegisterBase(Widget)
        ComponentTemplate({
            "WidgetDepthOrder": 10.0,
            "LayoutMode": "LayoutMode_Vertical",
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.PanelStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.PanelStylePadding}",
            "LayoutChildWeight": [0.0, 0.0],
            "LayoutChildOrder": [
                {
                    "LayoutChildOrderingProperty": "Property.Children"
                }
            ]
        })
    EndComponent()

    BeginComponent(MainMenu)
        RegisterBase(Menu)
        ComponentTemplate({
              "WidgetDepthOrder": 0.0,
              "RenderableSubMesh": "{SceneNodeScene.SceneStyle.PanelStyleMesh}",
              "LayoutMode": "LayoutMode_Horizontal",
              "LayoutChildWeight": [1.0, 0.0],
              "LayoutPadding": "{SceneNodeScene.SceneStyle.MainMenuStylePadding}"
          })
    EndComponent()

    BeginComponent(MenuItem)
        RegisterBase(InteractableWidget)
        RegisterProperty(StringRef, MenuItemTitle)
        RegisterChildProperty(TextWidget, MenuItemLabel)
        RegisterChildProperty(Menu, MenuItemSubMenu)
        ComponentTemplate({
          "RenderableSubMesh": "{SceneNodeScene.SceneStyle.MenuItemStyleMesh}",
          "LayoutMode": "LayoutMode_Horizontal",
          "LayoutPadding": "{SceneNodeScene.SceneStyle.ButtonStylePadding}",
          "LayoutChildWeight": [0.0, 0.0],
          "MenuItemLabel": {
            "TextWidgetFont": "{SceneNodeScene.SceneStyle.ButtonStyleFont}",
            "TextWidgetText": "{Owner.MenuItemTitle}",
            "TextWidgetColor": "{Owner.WidgetStateColor}"
          },
          "MenuItemSubMenu": {
              "Hidden": true
          },
          "LayoutChildOrder": [
              {
                  "LayoutChildOrderingProperty": "Property.MenuItemLabel"
              }
          ]
      })
    EndComponent()

    BeginComponent(MainMenuItem)
        RegisterBase(MenuItem)
        ComponentTemplate({
          "RenderableSubMesh": "{SceneNodeScene.SceneStyle.MainMenuItemStyleMesh}",
          "LayoutPadding": "{SceneNodeScene.SceneStyle.MenuItemStylePadding}",
          "LayoutChildWeight": [0.0, 0.0],
          "MenuItemLabel": {
                "TextWidgetFont": "{SceneNodeScene.SceneStyle.ButtonStyleFont}",
                "TextWidgetText": "{Owner.MenuItemTitle}"
            }
        })
    EndComponent()

    BeginComponent(VerticalSeparator)
        RegisterBase(Widget)
        ComponentTemplate({
          "RenderableSubMesh": "{SceneNodeScene.SceneStyle.SeparatorStyleVerticalMesh}",
          "LayoutPadding": "{SceneNodeScene.SceneStyle.SeparatorStylePadding}",
          "LayoutChildWeight": [1.0, 0.0]
        })
    EndComponent()

    BeginComponent(HorizontalSeparator)
        RegisterBase(Widget)
        ComponentTemplate({
              "RenderableSubMesh": "{SceneNodeScene.SceneStyle.SeparatorStyleHorizontalMesh}",
              "LayoutPadding": "{SceneNodeScene.SceneStyle.SeparatorStylePadding}",
              "LayoutChildWeight": [0.0, 1.0]
          })
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Size2D()), OnSize2DChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TransformHierarchyLevel()), OnMenuChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetDepthOrder()), OnMenuChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClicked()), OnClickedChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetHovered()), OnHoveredChanged, 0)
EndUnit()