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

int main(int argc, char** argv) {
    Module modules[] = {

        // Plugins
        ModuleOf_BgfxRendering(),
        ModuleOf_Json(),
        ModuleOf_StbDecoders(),

        // App Modules
        ModuleOf_Player(),
        ModuleOf_Editor(),

        ModuleOf_Scene2D(),
        ModuleOf_Scene3D(),

        0
    };

    // Map virtual resource paths used by different libraries,
    // to all find their resources in the current working directory
    StringRef virtualPaths[] = {
        "res://imgui", "file://.",
        "res://scene2d", "file://.",
        "res://assets", "file://.",
        "res://shadercache", "file://./shadercache",
        0, 0,
        0
    };

    StringRef scenes[] = {
        "res://assets/game.json",
        0
    };

    StringRef sourceDirectory = GetParentFolder(CleanupPath(FormatString("file://%s", __FILE__)));
    if(IsFolder(sourceDirectory)) {
        virtualPaths[8] = "res://source";
        virtualPaths[9] = sourceDirectory;
        scenes[0] = "res://source/game.json";
    }

    return PlayerMain(argc, argv, modules, virtualPaths, scenes, "Test Game - Built " __DATE__ " " __TIME__);
}