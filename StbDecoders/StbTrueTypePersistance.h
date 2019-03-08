//
// Created by Kim on 19-10-2018.
//

#ifndef PLAZA_STBTRUETYPEPERSISTANCE_H
#define PLAZA_STBTRUETYPEPERSISTANCE_H

#include <Core/NativeUtils.h>

struct UnicodeRange {
    u16 UnicodeRangeStart;
    u16 UnicodeRangeEnd;
};

struct TrueTypeFont {
    float TrueTypeFontSize;
};

struct TrueTypeFontRange {
    Entity TrueTypeUnicodeRange;
};

Unit(StbTrueTypePersistance)
    Component(UnicodeRange)
        Property(u16, UnicodeRangeStart)
        Property(u16, UnicodeRangeEnd)

    Component(TrueTypeFontRange)
        ReferenceProperty(UnicodeRange, TrueTypeUnicodeRange)

    Component(TrueTypeFont)
        Property(float, TrueTypeFontSize)
        ArrayProperty(TrueTypeFontRange, TrueTypeFontRanges)

#endif //PLAZA_STBTRUETYPEPERSISTANCE_H
