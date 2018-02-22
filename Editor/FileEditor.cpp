//
// Created by Kim Johannsen on 24/01/2018.
//

#include <Core/String.h>
#include "FileEditor.h"


    struct FileEditor {
        Entity EditorFileStream;
    };

    DefineComponent(FileEditor)
    EndComponent()

    DefineService(FileEditor)
    EndService()

    DefineComponentProperty(FileEditor, Entity, EditorFileStream)

    static bool ServiceStart() {
        return true;
    }

    static bool ServiceStop() {
        return true;
    }
