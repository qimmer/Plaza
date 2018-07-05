//
// Created by Kim on 07/05/2018.
//

#ifndef PLAZA_RECTSEGMENT_H
#define PLAZA_RECTSEGMENT_H

#include <Core/Entity.h>

DeclareComponent(RectSegment)
DeclareComponentPropertyReactive(RectSegment, v2f, RectSegmentMinAlignment)
DeclareComponentPropertyReactive(RectSegment, v2f, RectSegmentMinOffset)
DeclareComponentPropertyReactive(RectSegment, v2f, RectSegmentMaxAlignment)
DeclareComponentPropertyReactive(RectSegment, v2f, RectSegmentMaxOffset)
DeclareComponentPropertyReactive(RectSegment, float, RectSegmentRadiusTopLeft)
DeclareComponentPropertyReactive(RectSegment, float, RectSegmentRadiusTopRight)
DeclareComponentPropertyReactive(RectSegment, float, RectSegmentRadiusBottomLeft)
DeclareComponentPropertyReactive(RectSegment, float, RectSegmentRadiusBottomRight)


#endif //PLAZA_RECTSEGMENT_H
