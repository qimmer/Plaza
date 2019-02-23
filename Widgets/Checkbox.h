//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_CHECKBOX_H
#define PLAZA_CHECKBOX_H

#include <Core/NativeUtils.h>

struct Checkbox {
    StringRef CheckboxTitle;
};

struct CheckboxStyle {
    Entity CheckboxStyleMesh;
    Entity CheckboxStyleFont;
    v4i CheckboxStylePadding;
};

#endif //PLAZA_CHECKBOX_H
