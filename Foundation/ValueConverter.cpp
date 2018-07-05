//
// Created by Kim on 12/05/2018.
//

#include "ValueConverter.h"

struct ValueConverter {
    Function ValueConverterConvertTo, ValueConverterConvertFrom;
};

DefineComponent(ValueConverter)
EndComponent()

DefineComponentPropertyReactive(ValueConverter, Function, ValueConverterConvertTo)
DefineComponentPropertyReactive(ValueConverter, Function, ValueConverterConvertFrom)
