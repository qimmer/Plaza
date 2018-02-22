//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_PROPERTYEDITOR_H
#define PLAZA_PROPERTYEDITOR_H

#include <Core/Service.h>

DeclareService(PropertyEditor)

bool GetPropertyEditorVisible();
void SetPropertyEditorVisible(bool value);

namespace ImGui {
    void ComponentContextMenu(Type componentType);
}

#endif //PLAZA_PROPERTYEDITOR_H
