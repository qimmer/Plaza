//
// Created by Kim on 04-11-2018.
//

#ifndef PLAZA_DRAGHANDLE_H
#define PLAZA_DRAGHANDLE_H

#include <Core/NativeUtils.h>

struct DragHandle {
    bool DragHandleDragging;
    Entity DragHandleTarget;
    v2f DragHandlePositionStart;
    v2i DragHandleInteractionStart;
};

struct ResizeHandle {
    bool ResizeHandleResizing;
    Entity ResizeHandleTarget;
    v2i ResizeHandleSizeStart, ResizeHandleInteractionStart;
};

Unit(DragHandle)
    Component(DragHandle)
        Property(bool, DragHandleDragging)
        Property(v2f, DragHandlePositionStart)
        Property(v2i, DragHandleInteractionStart)
        ReferenceProperty(Widget, DragHandleTarget)

    Component(ResizeHandle)
        Property(bool, ResizeHandleResizing)
        Property(v2i, ResizeHandleSizeStart)
        Property(v2i, ResizeHandleInteractionStart)
        ReferenceProperty(Widget, ResizeHandleTarget)


#endif //PLAZA_DRAGHANDLE_H
