//
// Created by Kim on 07/05/2018.
//

#ifndef PLAZA_PATHSEGMENT_H
#define PLAZA_PATHSEGMENT_H

#include <Core/Entity.h>

DeclareComponent(PathSegment)
DeclareComponentPropertyReactive(PathSegment, Entity, PathSegmentStrokeBrush)
DeclareComponentPropertyReactive(PathSegment, Entity, PathSegmentFillBrush)
DeclareComponentPropertyReactive(PathSegment, float, PathSegmentStrokeWidth)

#endif //PLAZA_PATHSEGMENT_H
