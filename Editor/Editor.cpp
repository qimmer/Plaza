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
            "$components": ["Component.VerticalLayout"],
            "Size2D": "{RenderTargetSize}",
            "RenderContextTitle": "Editor",
            "RenderTargetSize": [800, 600],
            "Children": [
                {
                    "$components": ["Component.MainMenu"],
                    "Children": [
                        {
                            "ButtonTitle": "File"
                        },
                        {
                            "ButtonTitle": "Edit"
                        },
                        {
                            "ButtonTitle": "View"
                        }
                    ]
                },
                {
                    "$components": ["Component.HorizontalLayout"],
                    "LayoutChildWeight": [1.0, 1.0]
                },
                {
                    "$components": ["Component.Panel"],
                    "LayoutChildMinimumSize": [32, 32]
                }
            ]
        }
    })
EndModule()