//
// Created by Kim Johannsen on 04-04-2018.
//

#ifndef PLAZA_LABEL_H
#define PLAZA_LABEL_H

#include <Core/Entity.h>

DeclareComponent(Label)
DeclareComponentPropertyReactive(Label, Entity, LabelFont)
DeclareComponentPropertyReactive(Label, StringRef, LabelText)

DeclareService(Label)

#endif //PLAZA_LABEL_H
