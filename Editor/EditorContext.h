//
// Created by Kim Johannsen on 31-03-2018.
//

#ifndef PLAZA_EDITORCONTEXT_H
#define PLAZA_EDITORCONTEXT_H

#include <Core/Entity.h>

DeclareComponent(EditorContext)
DeclareComponentPropertyReactive(EditorContext, bool, EditorContextVisible)

DeclareService(EditorContext)

#endif //PLAZA_EDITORCONTEXT_H
