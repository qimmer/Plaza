//
// Created by Kim on 19-10-2018.
//

#ifndef PLAZA_STBTRUETYPEPERSISTANCE_H
#define PLAZA_STBTRUETYPEPERSISTANCE_H

#include <Core/NativeUtils.h>

struct TrueTypeFont {
    float TrueTypeFontSize;
    Entity TrueTypeFontFont;
};

Unit(StbTrueTypePersistance)
    Component(TrueTypeFont)
        Property(float, TrueTypeFontSize)
        ChildProperty(Font, TrueTypeFontFont)

#endif //PLAZA_STBTRUETYPEPERSISTANCE_H
