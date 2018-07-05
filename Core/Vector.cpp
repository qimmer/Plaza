//
// Created by Kim Johannsen on 03/01/2018.
//

#include "Vector.h"
#include "Types.h"

API_EXPORT void __SetVectorAmount(
    unsigned int* num,
    unsigned int newNum,
    unsigned int* dynCap,
    unsigned int staCap,
    void **curData,
    void *staBuf,
    unsigned int elementSize
) {
    if(*curData == NULL) *curData = staBuf;
    if(*num == newNum) return;

    if(newNum > staCap) {
        if(*curData == staBuf) {
            *curData = calloc(newNum, elementSize);
            *dynCap = newNum;
            memcpy(*curData, staBuf, *num);
        } else {
            if(*dynCap < newNum) {
                *curData = realloc(*curData, newNum * elementSize);
                *dynCap = newNum;
            }

            if(newNum > *num) {
                memset((char*)*curData + *num, 0, newNum - *num);
            }
        }
    } else {
        if(*curData != staBuf) free(*curData);
        *dynCap = 0;
        *curData = staBuf;
    }

    *num = newNum;
}