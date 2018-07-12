//
// Created by Kim Johannsen on 04-04-2018.
//

#ifndef PLAZA_LABEL_H
#define PLAZA_LABEL_H

#include <Core/NativeUtils.h>

Unit(Label)
    Component(Label)
        Property(Entity, LabelFont)
        Property(StringRef, LabelText)
        Property(v2f, LabelAlignment)


#endif //PLAZA_LABEL_H
