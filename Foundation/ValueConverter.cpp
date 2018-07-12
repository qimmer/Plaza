//
// Created by Kim on 12/05/2018.
//

#include "ValueConverter.h"

struct ValueConverter {
    Entity ValueConverterConvertTo, ValueConverterConvertFrom;
};

BeginUnit(ValueConverter)
    BeginComponent(ValueConverter)
        RegisterProperty(Entity, ValueConverterConvertTo)
        RegisterProperty(Entity, ValueConverterConvertFrom)
    EndComponent()
EndUnit()

