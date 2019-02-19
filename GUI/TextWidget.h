//
// Created by Kim on 08-01-2019.
//

#ifndef PLAZA_TEXTWIDGET_H
#define PLAZA_TEXTWIDGET_H

#include <Core/NativeUtils.h>

struct TextWidget {
    StringRef TextWidgetText;
    Entity TextWidgetMesh;
    Entity TextWidgetFont;
    rgba32 TextWidgetColor;
};

Unit(TextWidget)
    Component(TextWidget)
        Property(StringRef, TextWidgetText)
        ChildProperty(Mesh, TextWidgetMesh)
        ReferenceProperty(Font, TextWidgetFont)
        Property(rgba32, TextWidgetColor)

#endif //PLAZA_TEXTWIDGET_H
