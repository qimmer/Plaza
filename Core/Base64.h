//
// Created by Kim Johannsen on 09-03-2018.
//

#ifndef PLAZA_BASE64_H
#define PLAZA_BASE64_H

#include "Types.h"

void Base64Encode(u8* src, u64 len, s8 *out);
void Base64Decode(StringRef src, u8 *out, u64 *out_len);

#endif //PLAZA_BASE64_H
