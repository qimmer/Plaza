//
// Created by Kim Johannsen on 20-03-2018.
//

#ifndef PLAZA_ScenePicker_H
#define PLAZA_ScenePicker_H

#include <Core/NativeUtils.h>

Unit(ScenePicker)
    Component(ScenePicker)

        Property(u8, ScenePickerLayers)
        Property(v2f, ScenePickerViewportLocation)
        Property(Entity, ScenePickerPickedEntity)

#endif //PLAZA_ScenePicker_H
