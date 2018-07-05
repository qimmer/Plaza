//
// Created by Kim Johannsen on 30-03-2018.
//

#include <Core/Hashing.h>

API_EXPORT u32 HashCreate(const void *key, u32 len, u32 seed) {
    u32 c1 = 0xcc9e2d51;
    u32 c2 = 0x1b873593;
    u32 r1 = 15;
    u32 r2 = 13;
    u32 m = 5;
    u32 n = 0xe6546b64;
    u32 h = 0;
    u32 k = 0;
    u8 *d = (u8 *) key; // 32 bit extract from `key'
    const u32 *chunks = NULL;
    const u8 *tail = NULL; // tail - last 8 bytes
    int i = 0;
    int l = len / 4; // chunk length

    h = seed;

    chunks = (const u32 *) (d + l * 4); // body
    tail = (const u8 *) (d + l * 4); // last 8 byte chunk of `key'

    // for each 4 byte chunk of `key'
    for (i = -l; i != 0; ++i) {
        // next 4 byte chunk of `key'
        k = chunks[i];

        // encode next 4 byte chunk of `key'
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        // append to hash
        h ^= k;
        h = (h << r2) | (h >> (32 - r2));
        h = h * m + n;
    }

    k = 0;

    // remainder
    switch (len & 3) { // `len % 4'
        case 3: k ^= (tail[2] << 16);
        case 2: k ^= (tail[1] << 8);

        case 1:
            k ^= tail[0];
            k *= c1;
            k = (k << r1) | (k >> (32 - r1));
            k *= c2;
            h ^= k;
    }

    h ^= len;

    h ^= (h >> 16);
    h *= 0x85ebca6b;
    h ^= (h >> 13);
    h *= 0xc2b2ae35;
    h ^= (h >> 16);

    return h;
}
