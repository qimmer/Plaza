//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Editor/EditorModule.h>
#include <Player/PlayerModule.h>
#include <Player/PlayerContext.h>
#include <Scene2D/Scene2DModule.h>
#include <BgfxRendering/BgfxModule.h>
#include <Scene3D/Scene3DModule.h>

int main(int argc, char** argv) {
    Module modules[] = {

        // Plugins
        ModuleOf_BgfxRendering(),

        // App Modules
        ModuleOf_Player(),
        ModuleOf_Editor(),

        ModuleOf_Scene2D(),
        ModuleOf_Scene3D(),

        0
    };

    return PlayerMain(argc, argv, modules, 0, 0, "Test Voxel - Built " __DATE__ " " __TIME__);
}