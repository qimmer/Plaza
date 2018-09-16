//
// Created by Kim on 02-08-2018.
//

#ifndef PLAZA_STD_H
#define PLAZA_STD_H

#include <stddef.h>
#include "Types.h"

char *strnstr(const char *haystack, const char *needle, size_t len);

StringRef ToLower(StringRef str);
StringRef ToUpper(StringRef str);

#endif //PLAZA_STD_H
