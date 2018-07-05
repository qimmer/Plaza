//
// Created by Kim on 07/05/2018.
//

#ifndef PLAZA_TEXTSEGMENT_H
#define PLAZA_TEXTSEGMENT_H

#include <Core/Entity.h>

DeclareComponent(TextSegment)
DeclareComponentPropertyReactive(TextSegment, StringRef, TextSegmentText)
DeclareComponentPropertyReactive(TextSegment, Entity, TextSegmentFont)
DeclareComponentPropertyReactive(TextSegment, float, TextSegmentFontSize)
DeclareComponentPropertyReactive(TextSegment, u8, TextSegmentBlur)
DeclareComponentPropertyReactive(TextSegment, v2f, TextSegmentAlignment)

#endif //PLAZA_TEXTSEGMENT_H
