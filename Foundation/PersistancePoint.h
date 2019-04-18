
#ifndef PersistancePoint_H
#define PersistancePoint_H

#include <Core/NativeUtils.h>

struct PersistancePoint {
    bool PersistancePointLoading, PersistancePointSaving, PersistancePointAsync, PersistancePointLoaded;
};

Unit(PersistancePoint)
    Component(PersistancePoint)
        Property(bool, PersistancePointAsync)
        Property(bool, PersistancePointLoading)
        Property(bool, PersistancePointLoaded)
        Property(bool, PersistancePointSaving)

#endif //PersistancePoint_H
