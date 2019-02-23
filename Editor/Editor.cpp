//
// Created by Kim on 16-01-2019.
//

#include <Foundation/AppLoop.h>
#include <Widgets/WidgetsModule.h>
#include <Widgets/MainFrame.h>
#include <Json/NativeUtils.h>
#include "Editor.h"

BeginUnit(Editor)
    BeginComponent(Editor)
        RegisterChildProperty(MainFrame, EditorMainFrame)
    EndComponent()
EndUnit()

BeginModule(Editor)
    RegisterDependency(Widgets)

    RegisterUnit(Editor)

    ModuleData({
        "EditorMainFrame": {
            "Size2D": "{RenderTargetSize}",
            "RenderContextTitle": "Editor",
            "RenderTargetSize": [800, 600],
            "LayoutMode": "LayoutMode_Vertical",
            "Children": [
                {
                    "$components": ["Component.MainMenu"],
                    "MenuItems": [
                        {
                            "$components": ["Component.MainMenuItem"],
                            "MenuItemTitle": "File",
                            "MenuItemSubMenu": {
                                "MenuItems": [
                                    {
                                        "MenuItemTitle": "New ..."
                                    },
                                    {
                                        "MenuItemTitle": "Open ..."
                                    },
                                    {
                                        "MenuItemTitle": "Quit"
                                    }
                                ]
                            }
                        },
                        {
                            "$components": ["Component.MainMenuItem"],
                            "MenuItemTitle": "Edit"
                        },
                        {
                            "$components": ["Component.MainMenuItem"],
                            "MenuItemTitle": "View"
                        }
                    ]
                },
                {
                    "LayoutMode": "LayoutMode_Horizontal",
                    "LayoutChildWeight": [1.0, 1.0],
                    "Children": [
                        {
                            "$components": ["Component.Panel"],
                            "LayoutChildWeight": [0.25, 1.0]
                        },
                        {
                            "LayoutChildWeight": [0.5, 1.0]
                        },
                        {
                            "$components": ["Component.Panel"],
                            "LayoutChildWeight": [0.25, 1.0]
                        }
                    ]
                },
                {
                    "$components": ["Component.Panel"],
                    "LayoutChildWeight": [1.0, 0.0]
                }
            ]
        }
    })
EndModule()