#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <Core/NativeUtils.h>

typedef u32 TestResult;

#define Verify(test, message, ...) if (!(test)) { printf(message, ##__VA_ARGS__); return -1; } while (0)

#define Success 0

Unit(Test)
    Component(Test)

#endif