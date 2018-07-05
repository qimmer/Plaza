//
// Created by Kim on 12/05/2018.
//

#ifndef PLAZA_VALUECONVERTER_H
#define PLAZA_VALUECONVERTER_H

#include <Core/Entity.h>

DeclareComponent(ValueConverter)
DeclareComponentPropertyReactive(ValueConverter, Function, ValueConverterConvertTo)
DeclareComponentPropertyReactive(ValueConverter, Function, ValueConverterConvertFrom)

#endif //PLAZA_VALUECONVERTER_H
