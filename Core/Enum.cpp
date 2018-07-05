//
// Created by Kim Johannsen on 20-03-2018.
//

#include <cstring>
#include "Enum.h"
#include "Types.h"


struct Enum {
    bool EnumCombinable;
};

struct EnumFlag {
    u32 EnumFlagValue;
};

BeginUnit(Enum)
    BeginComponent(Enum)
        RegisterProperty(bool, EnumCombinable)
    EndComponent()
    BeginComponent(EnumFlag)
        RegisterProperty(u32, EnumFlagValue)
    EndComponent()
EndUnit()
