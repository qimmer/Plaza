//
// Created by Kim on 07/05/2018.
//

#ifndef PLAZA_RECTSEGMENT_H
#define PLAZA_RECTSEGMENT_H

#include <Core/NativeUtils.h>

Unit(RectSegment)
    Component(RectSegment)
        Property(v2f, RectSegmentMinAlignment)
        Property(v2f, RectSegmentMinOffset)
        Property(v2f, RectSegmentMaxAlignment)
        Property(v2f, RectSegmentMaxOffset)
        Property(float, RectSegmentRadiusTopLeft)
        Property(float, RectSegmentRadiusTopRight)
        Property(float, RectSegmentRadiusBottomLeft)
        Property(float, RectSegmentRadiusBottomRight)


#endif //PLAZA_RECTSEGMENT_H
