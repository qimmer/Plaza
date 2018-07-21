//
// Created by Kim Johannsen on 03/01/2018.
//

#include "Vector.h"
#include "Types.h"
#include "Math.h"

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
            *dynBuf = calloc(newNum, elementSize);
            *dynCap = newNum;
            memcpy(*dynBuf, staBuf, *num * elementSize);
        } else {
            if(*dynCap < newNum) {
                *dynBuf = realloc(*dynBuf, newNum * elementSize);
                *dynCap = newNum;
            }

            if(newNum > *num) {
                memset((char*)*dynBuf + *num, 0, newNum - *num);
            }
        }
    } else {
        if(*dynBuf) {
            memcpy(staBuf, *dynBuf, Min(staCap, *dynCap) * elementSize);
            free(*dynBuf);
        }
        *dynCap = 0;
        *dynBuf = NULL;
    }

    *num = newNum;
}