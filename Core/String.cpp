//
// Created by Kim Johannsen on 03/01/2018.
//

#include <Core/String.h>
#include <Core/Pool.h>

#include <string.h>
#include <stdarg.h>
#include <cstring>
#include <memory>

static std::vector<String> buffer;

template<>
const char * ApiConvert(const String& str) {
    return str.c_str();
}

StringRef FormatString(StringRef format, ...) {
    char buffer[1024 * 16];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer,1024 * 16,format, args);
    va_end (args);

    return GetTempString(buffer);
}

StringRef GetTempString(StringRef tempString) {
    buffer.push_back((String)tempString);

    return buffer[buffer.size() - 1].c_str();
}

void FreeTempStrings() {
    buffer.clear();
}


