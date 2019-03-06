//
// Created by Kim on 25-02-2019.
//

#ifndef PLAZA_JSONBINDING_H
#define PLAZA_JSONBINDING_H

#include <Core/NativeUtils.h>

Function(BindByString, bool, Entity entity, Entity property, StringRef sourceBindingString)
StringRef GetBindingString(const struct Binding& binding);

#endif //PLAZA_JSONBINDING_H
