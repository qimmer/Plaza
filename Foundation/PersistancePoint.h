
#ifndef PersistancePoint_H
#define PersistancePoint_H

#include <Core/NativeUtils.h>

Unit(PersistancePoint)
    Component(PersistancePoint)
        Property(bool, PersistancePointAsync)
        Property(bool, PersistancePointLoading)
        Property(bool, PersistancePointLoaded)
        Property(bool, PersistancePointSaving)

    Function(LoadEntityPath, bool, StringRef pathToEnsureLoaded)

#endif //PersistancePoint_H
