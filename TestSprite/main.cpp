//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Editor/EditorModule.h>
#include <Player/PlayerModule.h>
#include <Player/PlayerContext.h>
#include <Scene2D/Scene2DModule.h>
#include <BgfxRendering/BgfxModule.h>

int main(int argc, char** argv) {
    Module modules[] = {

        // Plugins
        ModuleOf_BgfxRendering(),

        // App Modules
        ModuleOf_Player(),
        ModuleOf_Editor(),

        ModuleOf_Scene2D(),

        0
    };

    StringRef assets[] = {
        "scene.json",
        0
    };

    return PlayerMain(
            argc,
            argv,
            modules,
            0,
            assets,
            "Test Sprite - Built " __DATE__ " " __TIME__
    );
}