//
// Created by Kim Johannsen on 03/01/2018.
//

#ifndef PLAZA_STRING_H
#define PLAZA_STRING_H

#include <Core/Handle.h>

#include <EASTL/fixed_string.h>
#include <Core/Type.h>
#include <Core/Types.h>
#include <Core/Service.h>


using String = eastl::fixed_string<char, 64>;

template<>
const char * ApiConvert(const String* str);

#endif //PLAZA_STRING_H
