//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Foundation/FoundationModule.h>
#include <Rendering/RenderingModule.h>
#include <Bgfx/BgfxModule.h>
#include <ImGui/ImGuiModule.h>
#include "EditorModule.h"
#include "MainMenu.h"
#include "FileExplorer.h"
#include "EntityExplorer.h"
#include "PropertyEditor.h"


    DefineModule(Editor)
        ModuleDependency(Foundation)
        ModuleDependency(Rendering)
        ModuleDependency(ImGui)

        ModuleService(MainMenu)
        ModuleService(FileExplorer)
        ModuleService(EntityExplorer)
        ModuleService(PropertyEditor)
    EndModule()
