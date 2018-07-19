
#ifndef PersistancePoint_H
#define PersistancePoint_H

#include <Core/NativeUtils.h>

Unit(PersistancePoint)
    Component(PersistancePoint)
        Property(bool, PersistancePointAsync)
        Property(bool, PersistancePointLoaded)
        Property(bool, PersistancePointLoading, PropertyFlag_ReadOnly, PropertyFlag_Transient)
        Property(bool, PersistancePointSaving, PropertyFlag_ReadOnly, PropertyFlag_Transient)

    Function(Save, bool, Entity persistancePoint)
    Function(LoadEntityPath, bool, StringRef pathToEnsureLoaded)

#endif //PersistancePoint_H
