//
// Created by Kim Johannsen on 31-03-2018.
//

#ifndef PLAZA_EDITORVIEW_H
#define PLAZA_EDITORVIEW_H

#include <Core/Entity.h>

DeclareComponent(EditorView)
DeclareComponentPropertyReactive(EditorView, Entity, EditorViewVisibilityState)
DeclareComponentPropertyReactive(EditorView, Entity, EditorViewVisibilityCommand)
DeclareComponentPropertyReactive(EditorView, EntityHandler, EditorViewDrawFunction)

DeclareService(EditorView)

#endif //PLAZA_EDITORVIEW_H
