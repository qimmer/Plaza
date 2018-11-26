//
// Created by Kim on 16-11-2018.
//

#include <Json/NativeUtils.h>
#include <Foundation/FoundationModule.h>
#include <Rendering/RenderingModule.h>
#include <Scene/SceneModule.h>

#include "SpriteModule.h"
#include "Tilemap.h"
#include "Sprite.h"

BeginModule(Sprite)
    RegisterDependency(Foundation)
    RegisterDependency(Rendering)
    RegisterDependency(Scene)

    RegisterUnit(Tilemap)
    RegisterUnit(Sprite)
EndModule()
