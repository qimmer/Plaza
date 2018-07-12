//
// Created by Kim Johannsen on 24/01/2018.
//

#ifndef PLAZA_CODEEDITOR_H
#define PLAZA_CODEEDITOR_H

#include <Core/NativeUtils.h>


Unit(CodeEditor)
    Component(CodeEditor)
        Property(Entity, CodeEditorStream)

void EditCode(Entity stream);

#endif //PLAZA_CODEEDITOR_H
