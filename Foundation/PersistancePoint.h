
#ifndef PersistancePoint_H
#define PersistancePoint_H

#include <Core/NativeUtils.h>

Unit(PersistancePoint)
    Component(PersistancePoint)
        Property(bool, PersistancePointAsync)
        Property(bool, PersistancePointLoading)
        Property(bool, PersistancePointLoaded)
        Property(bool, PersistancePointSaving)

    Component(UnresolvedReference)
        ReferenceProperty(Property, UnresolvedReferenceProperty)
        Property(StringRef, UnresolvedReferenceUuid)

    Component(UnresolvedEntity)
        ArrayProperty(UnresolvedReference, UnresolvedReferences)

    Function(LoadEntityPath, bool, StringRef pathToEnsureLoaded)
    Function(ResolveReferences, bool)

#endif //PersistancePoint_H
