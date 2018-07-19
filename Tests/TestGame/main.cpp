//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Editor/EditorModule.h>
#include <Player/PlayerModule.h>
#include <Player/PlayerContext.h>
#include <Scene2D/Scene2DModule.h>
#include <BgfxRendering/BgfxModule.h>
#include <Scene3D/Scene3DModule.h>
#include <Json/JsonModule.h>
#include <File/Folder.h>
#include <Core/String.h>
#include <StbDecoders/StbDecodersModule.h>
#include <Rendering/RenderingModule.h>
#include <ImGui/ImGuiModule.h>
#include <Scene/SceneModule.h>
#include <Foundation/Stream.h>
#include <UI/UIModule.h>
#include <DataModel/DataModelModule.h>
#include <Lua/LuaModule.h>

int main(int argc, char** argv) {
    Module modules[] = {

        // Plugins
        ModuleOf_BgfxRendering(),
        ModuleOf_Json(),
        ModuleOf_StbDecoders(),
        ModuleOf_Lua(),

        // App Modules
        ModuleOf_Player(),
        ModuleOf_Editor(),

        ModuleOf_Rendering(),
        ModuleOf_Scene(),
        ModuleOf_ImGui(),
        ModuleOf_Scene2D(),
        ModuleOf_Scene3D(),
        ModuleOf_UI(),
        ModuleOf_DataModel(),

        0
    };

    // Map virtual resource paths used by different libraries,
    // to all find their resources in the current working directory
    StringRef virtualPaths[] = {
        "res://Assets", "file://Assets",
        "res://shadercache", "file://./shadercache",
        0, 0,
        0
    };

    StringRef scenes[] = {
        "res://Assets/testlua.json",
        0
    };

    char sourceFilePath[PathMax];
    snprintf(sourceFilePath, PathMax, "file://%s", __FILE__);
    CleanupPath(sourceFilePath);

    char sourceDirectory[PathMax];
    GetParentFolder(sourceFilePath, sourceDirectory, PathMax);
    if(IsFolder(sourceDirectory)) {
        virtualPaths[4] = "res://Source";
        virtualPaths[5] = sourceDirectory;
        scenes[0] = "res://Source/Assets/testlua.json";
    }

    return PlayerMain(argc, argv, modules, virtualPaths, scenes);
}