//
// Created by Kim on 15-09-2018.
//

#ifndef PLAZA_STRINGS_H
#define PLAZA_STRINGS_H

#include "Types.h"

StringRef Intern(StringRef str);
StringRef AddStringRef(StringRef sourceString);
void ReleaseStringRef(StringRef sourceString);
void CleanupStrings();

#endif //PLAZA_STRINGS_H
