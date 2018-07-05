//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Foundation/FoundationModule.h>
#include <Rendering/RenderingModule.h>
#include <BgfxRendering/BgfxModule.h>
#include <ImGui/ImGuiModule.h>
#include <Editor/Tools/FirstPersonCameraTool.h>
#include <Editor/Editors/Texture2DViewer.h>
#include "EditorModule.h"
#include "MainMenu.h"
#include "Editor/ToolWindows/EntityExplorer.h"
#include "Editor/ToolWindows/PropertyEditor.h"
#include "EditorStyle.h"
#include "Editor/Editors/SceneEditor.h"
#include "Editor/Editors/CodeEditor.h"
#include "EditorStandardCommands.h"
#include "EditorContext.h"
#include "EditorView.h"

DefineModule(Editor)
        ModuleDependency(Foundation)
        ModuleDependency(Rendering)
        ModuleDependency(ImGui)

        ModuleType(EntityExplorer)
        ModuleType(PropertyEditor)
        ModuleType(SceneEditor)
        ModuleType(CodeEditor)
        ModuleType(Texture2DViewer)
        ModuleType(EditorContext)
        ModuleType(EditorView)

        ModuleService(EditorStandardCommands)
        ModuleService(EditorStyle)
        ModuleService(MainMenu)
        ModuleService(EntityExplorer)
        ModuleService(PropertyEditor)
        ModuleService(SceneEditor)
        ModuleService(CodeEditor)
        ModuleService(Texture2DViewer)
        ModuleService(EditorContext)

        ModuleService(FirstPersonCameraTool)
EndModule()
