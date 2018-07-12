//
// Created by Kim Johannsen on 31-03-2018.
//

#ifndef PLAZA_EDITORVIEW_H
#define PLAZA_EDITORVIEW_H

#include <Core/NativeUtils.h>

Unit(EditorView)
    Component(EditorView)
        Property(bool, EditorViewVisible)
        Property(EntityHandler, EditorViewDrawFunction)

#endif //PLAZA_EDITORVIEW_H
