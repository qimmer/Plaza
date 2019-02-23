#ifndef PLAZA_TYPES_H
#define PLAZA_TYPES_H

#if defined(_WIN32) || defined(_WIN64)
#   if !defined(_MSVC_LANG)
#       define API_EXPORT __declspec(dllexport)
#   else
#       define API_EXPORT
#   endif
#else
#   define API_EXPORT __attribute__((visibility("default")))
#   define API_IMPORT
#endif


#if defined(_MSC_VER)
#  define ALIGN(X) __declspec(align(X))
#else
#  define ALIGN(X) __attribute((aligned(X)))
#endif

typedef const char* StringRef;
typedef void* NativePtr;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef u64 Entity;
typedef u8 Type;
typedef u64 Date;

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

typedef struct ALIGN(16) {
    v4f x, y, z, w;
} m4x4f;

typedef struct {
    union {
        struct {
            u8 r, g, b, a;
        };

        struct {
            u8 x, y, z, w;
        };

        u32 rgba;
    };

} rgba8;

typedef struct ALIGN(16) {

    union {
        struct {
            float r, g, b, a;
        };

        struct {
            float x, y, z, w;
        };
    };
} rgba32;

typedef struct {
    union {
        struct {
            u8 r, g, b, _padding;
        };

        struct {
            u8 x, y, z, w;
        };

        u32 rgbx;
    };
} rgb8;

typedef struct {
    union {
        struct {
            float r, g, b;
        };

        struct {
            float x, y, z;
        };
    };
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

enum {
    TypeOf_unknown = 0,

    TypeOf_void = 1,
    TypeOf_u8 = 2,
    TypeOf_u16 = 3,
    TypeOf_u32 = 4,
    TypeOf_u64 = 5,
    TypeOf_s8 = 6,
    TypeOf_s16 = 7,
    TypeOf_s32 = 8,
    TypeOf_s64 = 9,
    TypeOf_float = 10,
    TypeOf_double = 11,
    TypeOf_bool = 12,
    TypeOf_StringRef = 13,

    TypeOf_v2i = 14,
    TypeOf_v3i = 15,
    TypeOf_v4i = 16,

    TypeOf_v2f = 17,
    TypeOf_v3f = 18,
    TypeOf_v4f = 19,

    TypeOf_m3x3f = 20,
    TypeOf_m4x4f = 21,

    TypeOf_Entity = 22,
    TypeOf_Type = 23,

    TypeOf_rgba8 = 24,
    TypeOf_rgb8 = 25,

    TypeOf_rgba32 = 26,
    TypeOf_rgb32 = 27,

    TypeOf_Date = 28,

    TypeOf_Variant = 29,
    TypeOf_NativePtr = 30,

    TypeOf_MAX,
};

u32 GetTypeSize(Type type);
u32 GetTypeAlignment(Type type);
StringRef GetTypeName(Type type);
Type FindType(StringRef typeName);

#define u8_Default 0
#define u16_Default 0
#define u32_Default 0
#define u64_Default 0
#define s8_Default 0
#define s16_Default 0
#define s32_Default 0
#define s64_Default 0

#define float_Default 0.0f
#define double_Default 0.0
#define bool_Default false
#define StringRef_Default ""

#define v2i_Default {0, 0}
#define v3i_Default {0, 0, 0}
#define v4i_Default {0, 0, 0, 0}

#define v2f_Default {0.0f, 0.0f}
#define v3f_Default {0.0f, 0.0f, 0.0f}
#define v4f_Default {0.0f, 0.0f, 0.0f, 0.0f}

#define m3x3f_Default {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}
#define m4x4f_Default {{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};

#define Entity_Default 0
#define Type_Default 0

#define rgba8_Default {0}
#define rgb8_Default {0}

#define rgba32_Default {0}
#define rgb32_Default {0}

#define Date_Default 0

#define Variant_Default {0, 0}
#define NativePtr_Default NULL

#endif