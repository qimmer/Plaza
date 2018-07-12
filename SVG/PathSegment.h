//
// Created by Kim on 07/05/2018.
//

#ifndef PLAZA_PATHSEGMENT_H
#define PLAZA_PATHSEGMENT_H

#include <Core/NativeUtils.h>

Unit(PathSegment)
    Component(PathSegment)
        Property(Entity, PathSegmentStrokeBrush)
        Property(Entity, PathSegmentFillBrush)
        Property(float, PathSegmentStrokeWidth)

#endif //PLAZA_PATHSEGMENT_H
