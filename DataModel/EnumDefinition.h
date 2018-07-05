//
// Created by Kim on 29/04/2018.
//

#ifndef PLAZA_ENUMDEFINITION_H
#define PLAZA_ENUMDEFINITION_H

#include <Core/Entity.h>

DeclareComponent(EnumDefinition)
    DeclareComponentPropertyReactive(EnumDefinition, bool, EnumDefinitionBitFlagged)
    DeclareComponentChild(EnumDefinition, EnumDefinitionFlags)

DeclareComponent(EnumDefinitionFlag)
    DeclareComponentPropertyReactive(EnumDefinitionFlag, u32, EnumDefinitionFlagValue)

#endif //PLAZA_ENUMDEFINITION_H
