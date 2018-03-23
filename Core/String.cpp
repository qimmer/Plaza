//
// Created by Kim Johannsen on 03/01/2018.
//

#include <Core/String.h>
#include <Core/Pool.h>

#include <string.h>
#include <stdarg.h>
#include <cstring>
#include <memory>

template<>
const char * ApiConvert(const String* str) {
    return str->c_str();
}


