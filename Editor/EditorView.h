//
// Created by Kim Johannsen on 31-03-2018.
//

#ifndef PLAZA_EDITORVIEW_H
#define PLAZA_EDITORVIEW_H

#include <Core/Entity.h>

DeclareComponent(EditorView)
DeclareComponentPropertyReactive(EditorView, bool, EditorViewVisible)
DeclareComponentPropertyReactive(EditorView, EntityHandler, EditorViewDrawFunction)

#endif //PLAZA_EDITORVIEW_H
