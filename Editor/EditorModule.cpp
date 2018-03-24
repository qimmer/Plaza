//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Foundation/FoundationModule.h>
#include <Rendering/RenderingModule.h>
#include <BgfxRendering/BgfxModule.h>
#include <ImGui/ImGuiModule.h>
#include <Editor/Tools/FirstPersonCamera.h>
#include <Editor/Editors/Texture2DViewer.h>
#include "EditorModule.h"
#include "MainMenu.h"
#include "Editor/ToolWindows/FileExplorer.h"
#include "Editor/ToolWindows/EntityExplorer.h"
#include "PropertyEditor.h"
#include "EditorStyle.h"
#include "Editor/Editors/SceneEditor.h"
#include "Editor/Editors/CodeEditor.h"
#include "EditorStandardCommands.h"


DefineModule(Editor)
        ModuleDependency(Foundation)
        ModuleDependency(Rendering)
        ModuleDependency(ImGui)

        ModuleType(SceneEditor)
        ModuleType(CodeEditor)
        ModuleType(Texture2DViewer)

        ModuleService(EditorStandardCommands)
        ModuleService(EditorStyle)
        ModuleService(MainMenu)
        ModuleService(FileExplorer)
        ModuleService(EntityExplorer)
        ModuleService(PropertyEditor)
        ModuleService(SceneEditor)
        ModuleService(CodeEditor)
        ModuleService(Texture2DViewer)

        ModuleService(FirstPersonCameraTool)
    EndModule()
