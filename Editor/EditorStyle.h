//
// Created by Kim Johannsen on 04-03-2018.
//

#ifndef PLAZA_EDITORSTYLE_H
#define PLAZA_EDITORSTYLE_H

#include <Core/Service.h>

DeclareService(EditorStyle)

struct ImFont * GetMonospaceFont();
struct ImFont * GetUIFont();

#endif //PLAZA_EDITORSTYLE_H
