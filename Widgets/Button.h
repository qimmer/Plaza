//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_BUTTON_H
#define PLAZA_BUTTON_H

#include <Core/NativeUtils.h>

struct Button {
    StringRef ButtonTitle;
};

struct ButtonStyle {
    Entity ButtonStyleMesh;
    Entity ButtonStyleFont;
    v4i ButtonStylePadding;
};

Unit(Button)
    Component(Button)
        Property(StringRef, ButtonTitle)

    Component(ButtonStyle)
        ChildProperty(WidgetMesh, ButtonStyleMesh)
        ReferenceProperty(Font, ButtonStyleFont)
        Property(v4i, ButtonStylePadding)

#endif //PLAZA_BUTTON_H
