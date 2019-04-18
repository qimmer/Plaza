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
        BeginChildProperty(EditorMainFrame)
    EndChildProperty()
    EndComponent()
EndUnit()

BeginModule(Editor)
    RegisterDependency(Widgets)

    RegisterUnit(Editor)

    ModuleDataFile("file://Editor/Editor.json")
EndModule()