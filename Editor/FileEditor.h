//
// Created by Kim Johannsen on 24/01/2018.
//

#ifndef PLAZA_FILEEDITOR_H
#define PLAZA_FILEEDITOR_H

#include <Core/Type.h>
#include <Core/Entity.h>


    void RegisterFileEditor(StringRef fileExtension, Type editorComponentType);
    void UnregisterFileEditor(Type editorComponentType);

    Entity GetActiveFileEditor();
    Entity OpenEditor(Entity FileStream);

    DeclareComponent(FileEditor)
    DeclareService(FileEditor)

    DeclareComponentProperty(FileEditor, Entity, EditorFileStream)

#endif //PLAZA_FILEEDITOR_H
