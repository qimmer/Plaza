//
// Created by Kim Johannsen on 20-03-2018.
//

#ifndef PLAZA_EDITORSTANDARDCOMMANDS_H
#define PLAZA_EDITORSTANDARDCOMMANDS_H

#include <Input/InputState.h>

Entity GetNewCommand();
Entity GetOpenCommand();
Entity GetSaveCommand();
Entity GetCloseCommand();

Entity GetCutCommand();
Entity GetCopyCommand();
Entity GetPasteCommand();
Entity GetDeleteCommand();

Entity GetQuitCommand();

DeclareService(EditorStandardCommands)

#endif //PLAZA_EDITORSTANDARDCOMMANDS_H
