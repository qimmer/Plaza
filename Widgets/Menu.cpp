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

LocalFunction(OnSize2DChanged, void, Entity entity, v2i oldSize, v2i newSize) {
    if(HasComponent(entity, ComponentOf_MainMenuItem())) {
        auto menu = GetMenuItemSubMenu(entity);
        SetPosition2D(menu, {0.0f, (float)newSize.y});
    }
    else if(HasComponent(entity, ComponentOf_MenuItem())) {
        auto menu = GetMenuItemSubMenu(entity);
        SetPosition2D(menu, {(float)newSize.x, 0.0f});
    }
}

LocalFunction(OnClickedChanged, void, Entity entity, bool oldClicked, bool newClicked) {
    if(newClicked && HasComponent(entity, ComponentOf_MainMenuItem())) {
        auto menu = GetOwner(entity);
        u32 numItems = 0;
        auto menuItems = GetMenuItems(menu, &numItems);
        auto alreadySelected = GetWidgetSelected(entity);
        for(auto i = 0; i < numItems; ++i) {
            auto selected = menuItems[i] == entity && !alreadySelected;
            auto subMenu = GetMenuItemSubMenu(menuItems[i]);
            //(subMenu, !selected);
            SetWidgetSelected(menuItems[i], selected);
        }
    }
}

LocalFunction(OnHoveredChanged, void, Entity entity, bool oldHovered, bool newHovered) {
    if(newHovered && HasComponent(entity, ComponentOf_MenuItem())) {
        auto menu = GetOwner(entity);
        u32 numItems = 0;
        auto menuItems = GetMenuItems(menu, &numItems);

        bool anyMenuOpen = false;
        for(auto i = 0; i < numItems; ++i) {
            if(GetWidgetSelected(menuItems[i])) {
                anyMenuOpen = true;
                break;
            }
        }

        if(!anyMenuOpen) return;

        for(auto i = 0; i < numItems; ++i) {
            auto selected = menuItems[i] == entity;
            auto subMenu = GetMenuItemSubMenu(menuItems[i]);
            //SetHidden(subMenu, !selected);
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

    BeginComponent(Menu)
        RegisterBase(Widget)
        RegisterArrayProperty(MenuItem, MenuItems)
        ComponentTemplate({
            "LayoutMode": "LayoutMode_Vertical",
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.PanelStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.PanelStylePadding}",
            "LayoutChildOrder": [
                {
                    "LayoutChildOrderingProperty": "Property.MenuItems"
                }
            ]
        })
    EndComponent()

    BeginComponent(MainMenu)
        RegisterBase(Menu)
        ComponentTemplate({
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
            "TextWidgetText": "{Owner.MenuItemTitle}"
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
          "MenuItemLabel": {
                "TextWidgetFont": "{SceneNodeScene.SceneStyle.ButtonStyleFont}",
                "TextWidgetText": "{Owner.MenuItemTitle}"
            }
        })
    EndComponent()

    BeginComponent(Separator)
        RegisterBase(Widget)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Size2D()), OnSize2DChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClicked()), OnClickedChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetHovered()), OnHoveredChanged, 0)
EndUnit()