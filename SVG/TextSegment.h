//
// Created by Kim on 07/05/2018.
//

#ifndef PLAZA_TEXTSEGMENT_H
#define PLAZA_TEXTSEGMENT_H

#include <Core/NativeUtils.h>

Unit(TextSegment)
    Component(TextSegment)
        Property(StringRef, TextSegmentText)
        Property(Entity, TextSegmentFont)
        Property(float, TextSegmentFontSize)
        Property(u8, TextSegmentBlur)
        Property(v2f, TextSegmentAlignment)

#endif //PLAZA_TEXTSEGMENT_H
