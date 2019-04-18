//
// Created by Kim Johannsen on 03/01/2018.
//

#include "NativeUtils.h"
#include "Vector.h"
#include "Types.h"
#include "Algorithms.h"
#include "Debug.h"

#include <EASTL/map.h>

#ifdef _DEBUG
static eastl::map<unsigned int*, u32> numExpansions;
#endif

#define TempBufferCapacity USHRT_MAX

static Entity tempBuffer[TempBufferCapacity];
static u32 tempPtr = 0;

API_EXPORT Entity* TempAlloc(u32 amount) {
    Assert(0, amount <= (TempBufferCapacity - tempPtr));

    auto ptr = &tempBuffer[tempPtr];
    tempPtr += amount;

    return ptr;
}

API_EXPORT void TempFree() {
    tempPtr = 0;
}
