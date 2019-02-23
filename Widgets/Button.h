//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_BUTTON_H
#define PLAZA_BUTTON_H

#include <Core/NativeUtils.h>

struct Button {
    StringRef ButtonTitle;
    Entity ButtonLabel;
};

struct ButtonStyle {
    Entity ButtonStyleMesh;
    Entity ButtonStyleFont;
    v4i ButtonStylePadding;
    v4f ButtonStyleTextColor;
};

Unit(Button)
    Component(Button)
        Property(StringRef, ButtonTitle)
        ChildProperty(Entity, ButtonLabel)

    Component(ButtonStyle)
        ChildProperty(WidgetMesh, ButtonStyleMesh)
        ReferenceProperty(Font, ButtonStyleFont)
        Property(v4i, ButtonStylePadding)
        Property(v4f, ButtonStyleTextColor)

#endif //PLAZA_BUTTON_H
