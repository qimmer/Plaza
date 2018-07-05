//
// Created by Kim on 29/04/2018.
//

#include "EnumDefinition.h"

struct EnumDefinition {
    Entity EnumDefinitionFlags;
    bool EnumDefinitionBitFlagged;
};

struct EnumDefinitionFlag {
    u32 EnumDefinitionFlagValue;
};

DefineComponent(EnumDefinition)
EndComponent()
    DefineComponentPropertyReactive(EnumDefinition, bool, EnumDefinitionBitFlagged)
    DefineComponentChild(EnumDefinition, Hierarchy, EnumDefinitionFlags)

DefineComponent(EnumDefinitionFlag)
EndComponent()
    DefineComponentPropertyReactive(EnumDefinitionFlag, u32, EnumDefinitionFlagValue)
