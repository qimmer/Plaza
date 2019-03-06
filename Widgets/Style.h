//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_STYLE_H
#define PLAZA_STYLE_H

#include <Core/NativeUtils.h>

struct Style {
    Entity StyleSheet;
    Entity StyleIconFont;
};

struct StyledScene {
    Entity SceneStyle;
};

Unit(Style)
    Component(Style)
        ChildProperty(TextureAtlas, StyleSheet)
        ArrayProperty(Font, StyleFonts)
        ReferenceProperty(Font, StyleIconFont)

    Component(StyledScene)
        ReferenceProperty(Style, SceneStyle)

#endif //PLAZA_STYLE_H
