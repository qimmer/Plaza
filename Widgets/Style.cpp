//
// Created by Kim on 17-01-2019.
//

#include "Style.h"
#include <Rendering/Texture2D.h>
#include <Gui/Font.h>

BeginUnit(Style)
    BeginComponent(Style)
        BeginChildProperty(StyleSheet)
        EndChildProperty()
        RegisterArrayProperty(Font, StyleFonts)
        RegisterReferenceProperty(Font, StyleIconFont)
    EndComponent()
    BeginComponent(StyledScene)
        RegisterReferenceProperty(Style, SceneStyle)
    EndComponent()
EndUnit()