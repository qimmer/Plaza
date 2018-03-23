//
// Created by Kim Johannsen on 20-03-2018.
//

#ifndef PLAZA_ScenePicker_H
#define PLAZA_ScenePicker_H

#include <Core/Entity.h>

DeclareComponent(ScenePicker)
DeclareService(ScenePicker)

DeclareComponentPropertyReactive(ScenePicker, bool, ScenePickerActive)
DeclareComponentPropertyReactive(ScenePicker, u8, ScenePickerLayers)
DeclareComponentPropertyReactive(ScenePicker, v2f, ScenePickerViewportLocation)

Entity GetPickedEntity(Entity scenePicker);

#endif //PLAZA_ScenePicker_H
