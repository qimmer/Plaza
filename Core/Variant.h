//
// Created by Kim Johannsen on 04/01/2018.
//

#ifndef PLAZA_VARIANT_H
#define PLAZA_VARIANT_H

#include <Core/String.h>
#include <Core/Types.h>



    enum VariantType {
        VariantType_I1,
        VariantType_D1,
        VariantType_I2,
        VariantType_D2,
        VariantType_I3,
        VariantType_D3,
        VariantType_I4,
        VariantType_D4,
        VariantType_STRING,
        VariantType_BOOL
    };
    typedef struct {
        VariantType type;
        String string;
        union {
            s64 v1i;
            double v1d;
            v2i v2i;
            v3i v3i;
            v4i v4i;
            v2d v2d;
            v3d v3d;
            v4d v4d;
            bool boolean;
        };
    } Variant;
}
#endif //PLAZA_VARIANT_H
