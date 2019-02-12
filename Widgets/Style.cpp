//
// Created by Kim on 17-01-2019.
//

#include "Style.h"
#include <Rendering/Texture2D.h>
#include <Gui/Font.h>

BeginUnit(Style)
    BeginComponent(Style)
        RegisterChildProperty(TextureAtlas, StyleSheet)
        RegisterArrayProperty(Font, StyleFonts)
    EndComponent()
    BeginComponent(StyledScene)
        RegisterReferenceProperty(Style, SceneStyle)
    EndComponent()
EndUnit()