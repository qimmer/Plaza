//
// Created by Kim Johannsen on 03/01/2018.
//

#include "Vector.h"
#include "Types.h"
#include "Math.h"
#include "Debug.h"

#include <EASTL/map.h>

#ifdef _DEBUG
static eastl::map<unsigned int*, u32> numExpansions;
#endif

API_EXPORT void __SetVectorAmount(
    unsigned int* num,
    unsigned int newNum,
    unsigned int* dynCap,
    unsigned int staCap,
    void **dynBuf,
    void *staBuf,
    unsigned int elementSize
) {
    if(*num == newNum) return;

    if(newNum > staCap) {
        if(!*dynBuf) {

#ifdef _DEBUG
			numExpansions[num] = 0;
#endif
            auto newCap = UpperPowerOf2(newNum);
            *dynBuf = calloc(newCap, elementSize);
            *dynCap = newCap;
            memcpy(*dynBuf, staBuf, *num * elementSize);
        } else {
            if(*dynCap < newNum) {

#ifdef _DEBUG
				auto prevExpansions = numExpansions[num];
				numExpansions[num]++;
				auto expansions = numExpansions[num];

				if ((prevExpansions / 100) != (expansions / 100)) {
					Log(0, LogSeverity_Warning, "Array 100-expansion mark");
				}
#endif

                auto newCap = UpperPowerOf2(newNum);
                auto newBuf = calloc(newCap, elementSize);
                memcpy(newBuf, *dynBuf, *num * elementSize);

                free(*dynBuf);

                *dynBuf = newBuf;
                *dynCap = newCap;
            }
        }
    } else {
        if(*dynBuf) {
            memcpy(staBuf, *dynBuf, Min(staCap, *dynCap) * elementSize);
            free(*dynBuf);
        }
        *dynCap = 0;
        *dynBuf = NULL;

        if(newNum > *num) {
            memset((char*)staBuf + (*num * elementSize), 0, elementSize * (newNum - *num)); // Null all new elements added in the bigger array
        }
    }

    *num = newNum;
}