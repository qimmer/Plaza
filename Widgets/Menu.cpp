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
#include <Scene/Scene.h>

static void RepositionMenu(Entity menu, bool vertical) {
    if(HasComponent(menu, ComponentOf_Menu()) && !HasComponent(menu, ComponentOf_MainMenu())) {
        auto ownerLevel  = GetOwnership(menu).OwnerLevel;
        auto depthLevel = GetWidget(menu).WidgetDepthOrder;
        auto itemSize = GetRect2D(GetOwnership(menu).Owner).Size2D;

        auto transformData = GetTransform(menu);
        if(vertical) {
            transformData.Position3D = {0.0f, (float)itemSize.y, -ownerLevel - depthLevel};
        }
        else {
            transformData.Position3D = {(float)itemSize.x, 0.0f, -ownerLevel - depthLevel};
        }

        SetTransform(menu, transformData);
    }
}

static void RepositionMenuItemMenu(Entity item) {
    if(HasComponent(item, ComponentOf_MainMenuItem())) {
        auto menu = GetMenuItem(item).MenuItemSubMenu;
        RepositionMenu(menu, true);
    }
    else if(HasComponent(item, ComponentOf_MenuItem())) {
        auto menu = GetMenuItem(item).MenuItemSubMenu;
        RepositionMenu(menu, false);
    }
}

static void OnRect2DChanged(Entity entity, const Rect2D& oldData, const Rect2D& newData) {
    RepositionMenuItemMenu(entity);
}

static void OnWidgetChanged(Entity entity, const Menu& oldData, const Menu& newData) {
    RepositionMenu(entity, HasComponent(GetOwnership(entity).Owner, ComponentOf_MainMenuItem()));
}

static void OnInteractableWidgetChanged(Entity entity, const InteractableWidget& oldData, const InteractableWidget& newData) {
    if(!oldData.WidgetClicked && newData.WidgetClicked && HasComponent(entity, ComponentOf_MainMenuItem())) {
        auto menu = GetOwnership(entity).Owner;

        auto menuItems = GetAppNode(menu).Children;
        auto alreadySelected = GetWidget(entity).WidgetSelected;
        for(auto i = 0; i < menuItems.GetSize(); ++i) {
            auto selected = menuItems[i] == entity && !alreadySelected;
            auto subMenu = GetMenuItem(menuItems[i]).MenuItemSubMenu;
            SetVisibility(subMenu, {!selected});

            auto widgetData = GetWidget(menuItems[i]);
            if(selected != widgetData.WidgetSelected) {
                widgetData.WidgetSelected = selected;
                SetWidget(menuItems[i], widgetData);
            }
        }
    }

    if(!oldData.WidgetHovered && newData.WidgetHovered && HasComponent(entity, ComponentOf_MenuItem())) {
        auto menu = GetOwnership(entity).Owner;
        auto menuItems = GetAppNode(menu).Children;

        bool anyMenuOpen = false;
        for(auto i = 0; i < menuItems.GetSize(); ++i) {
            if(GetWidget(menuItems[i]).WidgetSelected) {
                anyMenuOpen = true;
                break;
            }
        }

        if(!anyMenuOpen) return;

        for(auto i = 0; i < menuItems.GetSize(); ++i) {
            auto selected = menuItems[i] == entity;
            auto subMenu = GetMenuItem(menuItems[i]).MenuItemSubMenu;
            SetVisibility(subMenu, {!selected});

            auto widgetData = GetWidget(menuItems[i]);
            if(selected != widgetData.WidgetSelected) {
                widgetData.WidgetSelected = selected;
                SetWidget(menuItems[i], widgetData);
            }
        }
    }
}

BeginUnit(Menu)
    BeginComponent(MenuItemStyle)
        BeginChildProperty(MenuItemStyleMesh)
        EndChildProperty()
        RegisterProperty(v4i, MenuItemStylePadding)
    EndComponent()

    BeginComponent(MainMenuItemStyle)
        BeginChildProperty(MainMenuItemStyleMesh)
    EndChildProperty()
    EndComponent()

    BeginComponent(MainMenuStyle)
        RegisterProperty(v4i, MainMenuStylePadding)
    EndComponent()

    BeginComponent(SeparatorStyle)
        RegisterProperty(v4i, SeparatorStylePadding)
        BeginChildProperty(SeparatorStyleVerticalMesh)
        EndChildProperty()
        BeginChildProperty(SeparatorStyleHorizontalMesh)
    EndChildProperty()
    EndComponent()

    BeginComponent(Menu)
        RegisterBase(Widget)
    EndComponent()

    BeginComponent(MainMenu)
        RegisterBase(Menu)
    EndComponent()

    BeginComponent(MenuItem)
        RegisterBase(InteractableWidget)
        RegisterProperty(StringRef, MenuItemTitle)
        BeginChildProperty(MenuItemLabel)
        EndChildProperty()
        BeginChildProperty(MenuItemSubMenu)
        EndChildProperty()
    EndComponent()

    BeginComponent(MainMenuItem)
        RegisterBase(MenuItem)
    EndComponent()

    BeginPrefab(Menu)
        PrefabJson({
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
    EndPrefab()

    BeginPrefab(MainMenu)
        PrefabJson({
           "WidgetDepthOrder": 0.0,
           "RenderableSubMesh": "{SceneNodeScene.SceneStyle.PanelStyleMesh}",
           "LayoutMode": "LayoutMode_Horizontal",
           "LayoutChildWeight": [1.0, 0.0],
           "LayoutPadding": "{SceneNodeScene.SceneStyle.MainMenuStylePadding}"
        })
    EndPrefab()

    BeginPrefab(MenuItem)
        PrefabJson({
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
    EndPrefab()

    BeginPrefab(MainMenuItem)
        PrefabJson({
            "RenderableSubMesh": "{SceneNodeScene.SceneStyle.MainMenuItemStyleMesh}",
            "LayoutPadding": "{SceneNodeScene.SceneStyle.MenuItemStylePadding}",
            "LayoutChildWeight": [0.0, 0.0],
            "MenuItemLabel": {
                 "TextWidgetFont": "{SceneNodeScene.SceneStyle.ButtonStyleFont}",
                 "TextWidgetText": "{Owner.MenuItemTitle}"
            }
        })
    EndPrefab()

    BeginPrefab(VerticalSeparator)
        PrefabJson({
            "$components": ["Component.Widget"],
          "RenderableSubMesh": "{SceneNodeScene.SceneStyle.SeparatorStyleVerticalMesh}",
          "LayoutPadding": "{SceneNodeScene.SceneStyle.SeparatorStylePadding}",
          "LayoutChildWeight": [1.0, 0.0]
        })
    EndComponent()

    BeginPrefab(HorizontalSeparator)
        PrefabJson({
               "$components": ["Component.Widget"],
              "RenderableSubMesh": "{SceneNodeScene.SceneStyle.SeparatorStyleHorizontalMesh}",
              "LayoutPadding": "{SceneNodeScene.SceneStyle.SeparatorStylePadding}",
              "LayoutChildWeight": [0.0, 1.0]
          })
    EndComponent()

    RegisterSystem(OnRect2DChanged, ComponentOf_Rect2D())
    RegisterSystem(OnWidgetChanged, ComponentOf_Widget())
    RegisterSystem(OnInteractableWidgetChanged, ComponentOf_InteractableWidget())
EndUnit()