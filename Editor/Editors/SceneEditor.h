//
// Created by Kim Johannsen on 19-03-2018.
//

#ifndef PLAZA_SCENEEDITOR_H
#define PLAZA_SCENEEDITOR_H

#include <Core/Entity.h>

DeclareService(SceneEditor)

DeclareComponent(SceneEditor)
DeclareComponentPropertyReactive(SceneEditor, Entity, SceneEditorScene)

void EditScene(Entity scene);

Entity GetCurrentSceneEditorCamera();

#endif //PLAZA_SCENEEDITOR_H
