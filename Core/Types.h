#ifndef PLAZA_TYPES_H
#define PLAZA_TYPES_H

#if defined(_MSC_VER)
#  define ALIGN(X) /* __declspec(align(X)) */
#else
#  define ALIGN(X) __attribute((aligned(X)))
#endif

template<typename T1, typename T2>
T1 ApiConvert(const T2& value) {
    return (T1)value;
};

typedef const char* StringRef;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef struct {
    float x, y;
} v2f;

typedef struct {
    float x, y, z;
} v3f;

typedef struct ALIGN(16) {
    float x, y, z, w;
} v4f;

typedef struct {
    double x, y;
} v2d;

typedef struct {
    double x, y, z;
} v3d;

typedef struct {
    double x, y, z, w;
} v4d;

typedef struct {
    s32 x, y;
} v2i;

typedef struct {
    s32 x, y, z;
} v3i;

typedef struct {
    s32 x, y, z, w;
} v4i;

typedef struct {
    v3f x, y, z;
} m3x3f;

typedef struct ALIGN(64) {
    v4f x, y, z, w;
} m4x4f;

typedef struct {
    u8 r, g, b, a;
} rgba8;

typedef struct {
    float r, g, b, a;
} rgba32;

typedef struct {
    u8 r, g, b;
} rgb8;

typedef struct {
    float r, g, b;
} rgb32;

static const m4x4f m4x4f_Identity = { {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };
static const m3x3f m3x3f_Identity = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };
static const v4f v4f_Zero = {0, 0, 0, 0};
static const v4f v4f_Identity = {0, 0, 0, 1};
static const v4f v4f_One = {1, 1, 1, 1};
static const v3f v3f_Zero = {0, 0, 0};
static const v3f v3f_One = {1, 1, 1};
static const v3f v2f_Zero = {0, 0};
static const v3f v2f_One = {1, 1};

#endif