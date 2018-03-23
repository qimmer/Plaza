//
// Created by Kim Johannsen on 20-03-2018.
//

#include <Rendering/Context.h>
#include "EditorStandardCommands.h"

Entity NewCommand, OpenCommand, SaveCommand, CloseCommand, CutCommand, CopyCommand, PasteCommand, DeleteCommand, QuitCommand;

DefineService(EditorStandardCommands)
EndService()

Entity GetNewCommand() {
    return NewCommand;
}

Entity GetOpenCommand() {
    return OpenCommand;
}

Entity GetSaveCommand() {
    return SaveCommand;
}

Entity GetCloseCommand() {
    return CloseCommand;
}

Entity GetCutCommand() {
    return CutCommand;
}

Entity GetCopyCommand() {
    return CopyCommand;
}

Entity GetPasteCommand() {
    return PasteCommand;
}

Entity GetDeleteCommand() {
    return DeleteCommand;
}

Entity GetQuitCommand() {
    return QuitCommand;
}

static bool ServiceStart() {
    Entity editorRoot = CreateHierarchy(0, ".editor");
    Entity root = CreateHierarchy(editorRoot, "StandardCommands");
    
    NewCommand = CreateInputState(root, "New");
    SetInputStateKey(NewCommand, KEY_N);
    SetInputStatePrimaryModifierKey(NewCommand, KEY_LEFT_CONTROL);

    OpenCommand = CreateInputState(root, "Open");
    SetInputStateKey(OpenCommand, KEY_O);
    SetInputStatePrimaryModifierKey(OpenCommand, KEY_LEFT_CONTROL);

    SaveCommand = CreateInputState(root, "Save");
    SetInputStateKey(SaveCommand, KEY_S);
    SetInputStatePrimaryModifierKey(SaveCommand, KEY_LEFT_CONTROL);

    CloseCommand = CreateInputState(root, "Close");

    CutCommand = CreateInputState(root, "Cut");
    SetInputStateKey(CutCommand, KEY_X);
    SetInputStatePrimaryModifierKey(CutCommand, KEY_LEFT_CONTROL);

    CopyCommand = CreateInputState(root, "Copy");
    SetInputStateKey(CopyCommand, KEY_C);
    SetInputStatePrimaryModifierKey(CopyCommand, KEY_LEFT_CONTROL);

    PasteCommand = CreateInputState(root, "Paste");
    SetInputStateKey(PasteCommand, KEY_V);
    SetInputStatePrimaryModifierKey(PasteCommand, KEY_LEFT_CONTROL);

    DeleteCommand = CreateInputState(root, "Delete");
    SetInputStateKey(DeleteCommand, KEY_DELETE);

    PasteCommand = CreateInputState(root, "Quit");

    return true;
}

static bool ServiceStop() {
    return true;
}