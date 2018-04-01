//
// Created by Kim Johannsen on 31-03-2018.
//

#ifndef PLAZA_EDITORCONTEXT_H
#define PLAZA_EDITORCONTEXT_H

#include <Core/Entity.h>

DeclareComponent(EditorContext)
DeclareComponentPropertyReactive(EditorContext, Entity, EditorContextVisibilityState)
DeclareComponentPropertyReactive(EditorContext, Entity, EditorContextVisibilityCommand)

DeclareService(EditorContext)

#endif //PLAZA_EDITORCONTEXT_H
