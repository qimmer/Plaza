//
// Created by Kim on 16-01-2019.
//

#ifndef PLAZA_EDITOR_H
#define PLAZA_EDITOR_H

#include <Core/NativeUtils.h>

struct Editor {
    Entity EditorMainFrame;
};

Module(Editor)

Unit(Editor)
    Component(Editor)
        ChildProperty(MainFrame, EditorMainFrame)

#endif //PLAZA_EDITOR_H
